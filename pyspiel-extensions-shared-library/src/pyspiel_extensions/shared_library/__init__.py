__import__('pyspiel')    # we import pyspiel just for side effects
from ._core import load_game

__all__ = [
    "load_game"
]
