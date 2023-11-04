### ModBot - Swerve Algorithm
## Environment setup
For setting up any python project and to avoid "It works on my machine"! conversations..

The following commands are for Debian based systems, but should work apporpriatley with WSL (Windows), OSX, and other Linux distros.

Step 0: Make sure repo is cloned or updated

Step 1: ```sudo -s``` to enter root user.

Step 2: Install poetry ```curl -sSL https://install.python-poetry.org | python3 -```

Step 3: Add to path ```export PATH="/root/.local/bin:$PATH"```

Step 3.5: Check it installed correctly ```poetry --version```

Step 4: Navigate to the same directory as *pyproject.toml* and install the local enviornment ```poetry install```

Step 5: Run the sim ```poetry run python3 src/main.py```

Documentation on poetry, its very popular in industry and in the open source scene: https://python-poetry.org/docs/

