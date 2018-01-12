#include <map>
#include <string>
#include <vector>

#include <openmm/Context.h>
#include <openmm/Force.h>
#include <openmm/Vec3.h>

#include <openmm/internal/ContextImpl.h>
#include <openmm/internal/ForceImpl.h>

#include <openmm/reference/ReferencePlatform.h>

class MyForce : public OpenMM::Force
{
    class MyForceImpl : public OpenMM::ForceImpl
    {
      public:
        explicit MyForceImpl(MyForce const& owner)
            : owner_{owner}
        {
        }

        void initialize(OpenMM::ContextImpl&) override
        {
        }

        MyForce const& getOwner() const override
        {
            return owner_;
        }

        double calcForcesAndEnergy(OpenMM::ContextImpl& context, bool, bool, int) override
        {
            auto const springConstant = 1.0;

            auto const numParticles = context.getSystem().getNumParticles();
            auto const* data
                = static_cast<OpenMM::ReferencePlatform::PlatformData*>(context.getPlatformData());

            auto const& positions = *static_cast<std::vector<OpenMM::Vec3>*>(data->positions);
            auto& forces = *static_cast<std::vector<OpenMM::Vec3>*>(data->forces);
            double energy = 0;

            for (int i = 0; i < numParticles; ++i) {
                // Harmonic potential force.
                forces[i] += positions[i] * springConstant;
                energy += positions[i].dot(positions[i]);
            }
            energy *= springConstant / 2;

            return energy;
        }

        void updateContextState(OpenMM::ContextImpl&) override
        {
        }

        std::map<std::string, double> getDefaultParameters() override
        {
            return {};
        }

        std::vector<std::string> getKernelNames() override
        {
            return {};
        }

      private:
        MyForce const& owner_;
    };

  protected:
    OpenMM::ForceImpl* createImpl() const override
    {
        return new MyForceImpl{*this};
    }
};

//------------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include <OpenMM.h>

struct SimulationConfig
{
    double const temperature = 300.0; // kelvin
    double const thermal_energy = 0.00831445986 * temperature; // kJ/mol
    double const lennard_jones_epsilon = 2 * thermal_energy;
    double const lennard_jones_sigma = 1.0; // nm
    double const particle_mass = 100; // dalton
    double const particle_charge = 0;
    double const step_size = 0.1; // ps
    double const friction = 50.0; // 1/ps

    int const particle_count = 100;
    double const initial_distribution_sigma = 10.0; // nm

    int const frame_step_count = 10000;
    int const frame_count = 100;
};

class Simulation : private SimulationConfig
{
  public:
    Simulation()
    {
        system_.addForce(new MyForce);

        for (int i = 0; i < particle_count; ++i) {
            system_.addParticle(particle_mass);
        }
    }

    void run()
    {
        OpenMM::LangevinIntegrator integrator{temperature, friction, step_size};
        OpenMM::Context context{system_, integrator};

        init_positions(context);
        for (int frame_index = 0; frame_index < frame_count; ++frame_index) {
            show_progress(context, frame_index);
            integrator.step(frame_step_count);
        }
        dump_positions(context);
    }

  private:
    void init_positions(OpenMM::Context& context)
    {
        std::normal_distribution<double> coord{0.0, initial_distribution_sigma};
        std::vector<OpenMM::Vec3> positions;
        std::generate_n(std::back_inserter(positions), particle_count, [&] {
            return OpenMM::Vec3{coord(engine_), coord(engine_), coord(engine_)};
        });
        context.setPositions(positions);
    }

    void show_progress(OpenMM::Context const& context, int frame_index) const
    {
        auto const& state = context.getState(OpenMM::State::Energy);
        std::clog << frame_index << '\t'
                  << state.getKineticEnergy() / particle_count / thermal_energy << '\t'
                  << state.getPotentialEnergy() / particle_count / thermal_energy << '\n';
    }

    void dump_positions(OpenMM::Context const& context) const
    {
        auto const& state = context.getState(OpenMM::State::Positions);
        for (OpenMM::Vec3 pos : state.getPositions()) {
            std::cout << pos[0] << '\t' << pos[1] << '\t' << pos[2] << '\n';
        }
    }

  private:
    OpenMM::System system_;
    std::mt19937_64 engine_;
};

int main()
{
    Simulation simulation;
    simulation.run();
}
