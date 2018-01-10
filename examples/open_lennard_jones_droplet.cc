#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include <OpenMM.h>

int main()
{
    double const temperature = 300.0; // kelvin
    double const thermal_energy = 0.00831445986 * temperature; // kJ/mol
    double const lennard_jones_epsilon = 2 * thermal_energy;
    double const lennard_jones_sigma = 1.0; // nm
    double const particle_mass = 1000; // dalton
    double const particle_charge = 0;
    double const step_size = 0.1; // ps
    double const friction = 50.0; // 1/ps

    int const particle_count = 100;
    double const initial_distribution_sigma = 10.0; // nm

    int const frame_step_count = 10000;
    int const frame_count = 100;

    auto init_positions = [&](OpenMM::Context& context, std::mt19937_64& engine) {
        std::vector<OpenMM::Vec3> positions(static_cast<std::size_t>(particle_count));
        std::normal_distribution<double> coord{0.0, initial_distribution_sigma};
        std::generate(std::begin(positions), std::end(positions), [&] {
            return OpenMM::Vec3{coord(engine), coord(engine), coord(engine)};
        });
        context.setPositions(positions);
    };

    auto show_progress = [&](OpenMM::Context const& context, int frame_index) {
        auto const& state = context.getState(OpenMM::State::Energy);
        std::clog << frame_index << '\t'
                  << state.getKineticEnergy() / particle_count / thermal_energy << '\t'
                  << state.getPotentialEnergy() / particle_count / thermal_energy << '\n';
    };

    auto dump_positions = [&](OpenMM::Context const& context) {
        auto const& state = context.getState(OpenMM::State::Positions);
        for (OpenMM::Vec3 pos : state.getPositions()) {
            std::cout << pos[0] << '\t' << pos[1] << '\t' << pos[2] << '\n';
        }
    };

    std::mt19937_64 engine;
    OpenMM::System system;

    auto& nonbonded = *new OpenMM::NonbondedForce{};
    system.addForce(&nonbonded);

    for (int i = 0; i < particle_count; ++i) {
        system.addParticle(particle_mass);
        nonbonded.addParticle(particle_charge, lennard_jones_sigma, lennard_jones_epsilon);
    }

    OpenMM::LangevinIntegrator integrator{temperature, friction, step_size};
    OpenMM::Context context{system, integrator};

    init_positions(context, engine);
    for (int frame_index = 0; frame_index < frame_count; ++frame_index) {
        show_progress(context, frame_index);
        integrator.step(frame_step_count);
    }
    dump_positions(context);
}
