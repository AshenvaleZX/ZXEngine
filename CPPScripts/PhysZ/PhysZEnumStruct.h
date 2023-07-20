#pragma once

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		struct PotentialContact
		{
			RigidBody* bodies[2];
		};
	}
}