<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
<div align="center">

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![Latest Release][release-shield]][release-url]

---

[![Build][build-shield]][build-url]
[![Linting][lint-shield]][lint-url]
[![Docs][docs-shield]][docs-url]
[![Coverage][coverage-shield]][coverage-url]


</div>



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/NicolasReboule/Dodo">
    <img src="assets/logo.ico" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Dodo</h3>

  <p align="center">
    Dodo is a C++ Vulkan Graphics Library <br />
    <br />
    <a href="https://NicolasReboule.github.io/Dodo"><strong>Explore the source code docs »</strong></a>
    <br />
    <a href="https://github.com/NicolasReboule/Dodo/issues">Report Bug</a>
    ·
    <a href="https://github.com/NicolasReboule/Dodo/issues">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#documentation">Documentation</a></li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#authors">Authors</a></li>
  </ol>
</details>

## About The Project

I am building the library as I learn Vulkan following the documentation. So all the raw code and testing is in the main.cpp for learning and the cleaner code is in the sources but it may change as I learn more about Vulkan.

Project is built in C++ 23 as I wanted to try out std::expected to treat errors as values, that is why you will see "Create" methods instead of using constructors directly.

### Built With

* [![CPP][CPP]][CPP-url]
* [![Vulkan][Vulkan]][Vulkan-url]
* [![GLFW][GLFW]][GLFW-url]
* [![CMake][CMake]][CMake-url]
* [![CLion][CLion]][CLion-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## Getting Started

### Prerequisites

- CMake
- C++23
- Vulkan SDK

### Installation



## Documentation

The documentation of this project can be found at:
- Source code documentation: [https://nicolasreboule.github.io/Dodo][github.io-url]

## Usage

See the examples in the [source code documentation][github.io-url].

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

Read the [contributing guide][contributing-url] to learn how to contribute to the project.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- LICENSE -->
## License

Distributed under the MIT License. See [LICENSE][license-url] for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Authors

<table>
    <tbody>
        <tr>
            <td align="center"><a href="https://github.com/NicolasReboule/"><img src="https://avatars.githubusercontent.com/u/72016245?v=4?s=100" width="100px;" alt="Nicolas Reboule"/><br /><sub><b>Nicolas Reboule</b></sub></a><br /></td>
        </tr>
    </tbody>
</table>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/NicolasReboule/Dodo.svg?style=for-the-badge
[contributors-url]: https://github.com/NicolasReboule/Dodo/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/NicolasReboule/Dodo.svg?style=for-the-badge
[forks-url]: https://github.com/NicolasReboule/Dodo/network/members
[stars-shield]: https://img.shields.io/github/stars/NicolasReboule/Dodo.svg?style=for-the-badge
[stars-url]: https://github.com/NicolasReboule/Dodo/stargazers
[issues-shield]: https://img.shields.io/github/issues/NicolasReboule/Dodo.svg?style=for-the-badge
[issues-url]: https://github.com/NicolasReboule/Dodo/issues
[license-shield]: https://img.shields.io/github/license/NicolasReboule/Dodo.svg?style=for-the-badge
[license-url]: https://github.com/NicolasReboule/Dodo/blob/master/LICENSE
[contributing-url]: CONTRIBUTING.md
[github.io-url]: https://NicolasReboule.github.io/Dodo/
[release-shield]: https://img.shields.io/github/v/release/NicolasReboule/Dodo?color=lime&label=LATEST%20RELEASE&style=for-the-badge
[release-url]: https://github.com/NicolasReboule/Dodo/releases/latest
[build-url]: https://github.com/NicolasReboule/Dodo/actions/workflows/build.yml
[build-shield]: https://img.shields.io/github/actions/workflow/status/NicolasReboule/Dodo/build.yml?style=for-the-badge
[lint-url]: https://github.com/NicolasReboule/Dodo/actions/workflows/linter.yml
[lint-shield]: https://img.shields.io/github/actions/workflow/status/NicolasReboule/Dodo/linter.yml?style=for-the-badge&label=Linting
[docs-url]: https://github.com/NicolasReboule/Dodo/actions/workflows/pages/pages-build-deployment
[docs-shield]: https://img.shields.io/github/actions/workflow/status/NicolasReboule/Dodo/pages/pages-build-deployment?style=for-the-badge&label=Docs
[coverage-url]: https://app.codecov.io/github/NicolasReboule/Dodo
[coverage-shield]: https://img.shields.io/codecov/c/github/NicolasReboule/Dodo?token=0Z3FKJ3XLE&style=for-the-badge


[CPP]: https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white
[CPP-url]: https://en.cppreference.com/w/cpp/20
[CMake]: https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white
[CMake-url]: https://cmake.org
[CLion]: https://img.shields.io/badge/CLion-black?style=for-the-badge&logo=clion&logoColor=white
[CLion-url]: https://www.jetbrains.com/clion/
[Vulkan]: https://img.shields.io/badge/Vulkan-AE0F28?logo=Vulkan&logoColor=fff&style=for-the-badge
[Vulkan-url]: https://www.vulkan.org
[GLFW]: https://img.shields.io/badge/GLFW-ff7f13?style=for-the-badge
[GLFW-url]: https://www.glfw.org
