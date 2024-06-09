<p align="center">
  <img src="nexlogo.png" alt="NexOS Logo" width="400">
</p>

NexOS is a 32-bit operating system. Instead of traditional Makefiles, it utilizes Python scripts for its build process.

## Contents

- [Overview](#overview)
- [Directory Structure](#directory-structure)
- [Building NexOS](#building-nexos)
- [Running NexOS](#running-nexos)
- [Contributing](#contributing)
- [License](#license)

## Overview

NexOS is a lightweight operating system designed for educational and experimental purposes. It's written in a mix of assembly language and C, with a focus on simplicity and clarity of code. This repository contains the complete source code and build scripts necessary to compile and run NexOS.

## Directory Structure

The repository is organized as follows:

- `NexOS/`: Root directory containing all source code, build scripts, and auxiliary files.
  - `build/`: Compiled object files.
  - `inc/`: Header files.
  - `isobuild/`: Files necessary for ISO creation.
  - `src/`: Source code for kernel components.

## Building NexOS

To build NexOS, ensure Python3 is installed on your system. Then, navigate to the `NexOS/` directory and run the `run.py` script. This will compile the source code and generate the necessary object files.

## Running NexOS

Once built, run.pu automatically runs NexOS with QEMU

## Contributing

Contributions to NexOS are welcome! If you'd like to contribute, please fork this repository, make your changes, and submit a pull request.

## License

NexOS is licensed under the [MIT License](LICENSE). Feel free to use, modify, and distribute the code as you see fit.

---

For more detailed information on specific files and functionalities, refer to the comments within the source code. Happy exploring!
