# This stub describes the compiled C++ module for static analysis tools.
import pyspiel

# Define the function(s) that exist inside _core.so.
def load_game(game_name: str, params: dict | None = None) -> pyspiel.Game: ...
