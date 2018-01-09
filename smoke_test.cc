#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

#include <OpenMM.h>

template<typename Force, typename... Args>
Force& add_force(OpenMM::System& system, Args&&... args)
{
    auto force = std::make_unique<Force>(std::forward<Args>(args)...);
    system.addForce(force.get());
    return *force.release(); // The pointer is managed by the system.
}

int main()
{
    OpenMM::System system;
    auto& nonbonded = add_force<OpenMM::NonbondedForce>(system);
    system.addParticle(1.0);
    nonbonded.addParticle(0.0, 1.0, 1.0);
    OpenMM::VerletIntegrator integrator(1.0);
    OpenMM::Context context(system, integrator);
    OpenMM::State state = context.getState(OpenMM::State::Positions);
    assert(state.getPositions().size() == 1);
    std::cout << "OK\n";
}
