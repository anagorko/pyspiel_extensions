import pytest

import pyspiel
from pyspiel_extensions import shared_library

# The short_name defined in the C++ kGameType for your custom game.
CUSTOM_GAME_NAME = "subtraction_game"


def test_custom_game_is_loaded_by_shared_library():
    """
    Tests that shared_library.load_game successfully finds and loads
    the custom game. This proves the game is registered within
    the scope of our custom library.
    """
    print(f"Attempting to load '{CUSTOM_GAME_NAME}' from shared_library...")
    game = shared_library.load_game(CUSTOM_GAME_NAME)

    # Assert that a valid game object was returned and has the correct name.
    assert game is not None
    assert game.get_type().long_name == "Subtraction Game"
    print("Success!")


def test_standard_game_is_also_loaded_by_shared_library():
    """
    Tests that shared_library.load_game can ALSO load a standard game.
    This acts as a control, proving that our library's internal registry
    was successfully initialized with all of OpenSpiel's default games.
    """
    print("Attempting to load 'tic_tac_toe' from shared_library...")
    game = shared_library.load_game("tic_tac_toe")

    # Assert that it loaded the correct standard game.
    assert game is not None
    assert game.get_type().long_name == "Tic Tac Toe"
    print("Success!")


def test_custom_game_is_not_in_pyspiel_registry():
    """
    This is the key test. It asserts that a call to the main pyspiel.load_game
    with our custom game name FAILS with an exception. This proves the two
    registries are separate and isolated.
    """
    print(f"Attempting to load '{CUSTOM_GAME_NAME}' from the main pyspiel library...")

    # Use pytest.raises to assert that an exception is thrown.
    # The 'with' block will pass only if the expected exception occurs.
    with pytest.raises(Exception) as excinfo:
        pyspiel.load_game(CUSTOM_GAME_NAME)

    # Optionally, assert that the error message contains the expected text.
    # This makes the test more robust.
    assert "Unknown game" in str(excinfo.value)
    print("Failed as expected! The registries are separate.")
