#include <cstddef>
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
            auto const* data = static_cast<OpenMM::ReferencePlatform::PlatformData*>(context.getPlatformData());

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
            return {"reference"};
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

int main()
{
}
