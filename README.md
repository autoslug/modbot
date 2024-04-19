# VSCode Pico SDK Base Project

## Getting started

See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started) and the README in the [pico-sdk](https://github.com/raspberrypi/pico-sdk) for information
on getting up and running.

### Running VSCode
Upon installation of the Pico SDK, a new shortcut called Pico - Visual Studio Code should be present on your computer. **It is very important that you run this shortcut to open vscode** It contains information about environment variables that your normal vscode installation does not have. Your cmake build WILL FAIL.

### Configuring Cmake

 - Install the Cmake extension
 - Open CMake extension settings
   - Under the **Configure Environment** section create a new variable called
   PICO_SDK_PATH (the left box) and set it equal to the directory where the pico sdk is installed. On Windows machines, this would usually be: C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk
   - Under the **Preferred Generators** section, click the *Edit in settings.json* button and add this line to the cmake.preferredGenerators list: "NMake Makefiles"
   - Save your changes and exit settings

### Pulling from the GitHub repository
Since you are reading this file, its safe to assume that you have already done this step. If you have not, perform a pull. **Make sure to keep the file structure the exact same, or else you will break the Cmake build!!!** Place all your source code in the src directory. The Cmake extension should automatically build your files upon startup.

If the build ran correctly, you should see a /build directory that was created. This directory is part of the gitignore FOR A REASON.

### Flashing the Pico
If all the above steps ran successfully, you should be able to see buttons on the bottom left of your workspace called Build and a symbol that looks like a run button. The build button will run cmake build . and make all your files. The run button will create a .elf file that you can them move onto the pico.
