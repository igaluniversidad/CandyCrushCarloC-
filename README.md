# SDL3 DrawReady Template

![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)
![SDL3](https://img.shields.io/badge/SDL-3.4-green.svg)
![Status](https://img.shields.io/badge/Status-Active-success.svg)

This is a ready-to-use C++ template pre-configured with **SDL3**, **SDL3_image**, and **SDL3_ttf**. The project has all the linker settings, include directories, and external libraries properly set up so you can start drawing on the screen right away.

### Features
* **Wrapper Classes:** Clean implementations for rendering images, managing text, and handling colors.
* **State Management:** A lightweight GameState system with `Init`, `Draw`, and `Update` loops, including a sample `Game` state to help you get started.
* **Custom Data Structures:** Includes foundational data structures built from scratch, perfect for understanding memory management and extending functionality.
* **Automated Build Process:** No manual DLL copying required! Post-build events handle the deployment of all necessary dynamic libraries automatically.

### Getting Started

1. Clone the repository to your local machine.
2. Open the solution (`.sln`) in Visual Studio.
3. Select your preferred architecture (x64 or x86) and build the project.
4. Run it! 

*Note: The post-build events will automatically copy all the required DLLs (SDL3, image, ttf, webp, etc.) from the `ExternalLibs` folder directly to your output `Debug` or `Release` directory.*

---

Feel free to use this project as a structural foundation for your own games or as an educational resource to understand how SDL3 works under the hood. Contributions and improvements are always welcome!