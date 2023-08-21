# modbot

## Development
The `main` branch will be used as "known good" code — anything in the top-level `/pi` or `/pico` folders will be considered (more or less) bug-free and safe to run on the robot. All changes to main must be made via pull requests, almost always from `dev`. The `dev` branch would be where smaller development comes together, typically under the `/dev` folder. For example, various swerve-related experiments can be stored in `/dev/swerve`. However, the preferred development workflow would be the following:
1. Create a branch off of `dev`, with a descriptive name.
2. If working on an experiment (something that's not directly code for the Pi or one of the Picos), store it in a folder in `/dev`.
3. If working on a "final" program for the Pi or one of the Picos, store it in the `/pi` or `/pico` folder as appropriate.
4. After you're done, rebase your branch onto `dev`, then create a pull request from your branch to `dev`, and (preferably), get it reviewed before you merge it.
  a. If there are changes on `dev` before you merge your pull request, please rebase again.
  b. Your branch doesn't have to be deleted, but you should merge `dev` into your branch if you don't delete it.
5. Once the code in `/pi` and `/pico` on the `dev` branch is bug-free, safe to run on the robot, and the features in development are complete, create a pull request from the `dev` branch to the `main` branch. This must be reviewed by at least one other person, before it can be merged. Once it is merged, merge `main` back into `dev`.
