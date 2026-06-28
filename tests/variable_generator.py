import random

import colapy


class VariableGenerator(colapy.GeneratorBase):
    def __init__(
        self,
        *,
        min_particles: int = 0,
        max_particles: int = 50,
        seed: int = 42,
        **extra: str,
    ) -> None:
        _ = extra
        self._min = int(min_particles)
        self._max = int(max_particles)
        self._rng = random.Random(int(seed))

    def __call__(self) -> colapy.EventData:
        state = colapy.EventInitialState(
            pdg_code_a=1000010020,
            pdg_code_b=1000010020,
            energy=10.0,
        )

        count = self._rng.randint(self._min, self._max)
        particles = []
        for _ in range(count):
            particle = colapy.Particle(
                position=colapy.LorentzVector(),
                momentum=colapy.LorentzVector(
                    e=1.0,
                    x=self._rng.uniform(-2.0, 2.0),
                    y=self._rng.uniform(-2.0, 2.0),
                    z=self._rng.uniform(-2.0, 2.0),
                ),
                pdg_code=211,
                p_class=colapy.ParticleClass.PRODUCED,
            )
            particles.append(particle)

        return colapy.EventData(state, particles)
