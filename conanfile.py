from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


class IoLibConan(ConanFile):
    name = "io-lib"
    version = "1.0.0-2"
    package_type = "library"
    user = "dochkas"
    channel = "experimental"

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "platform": ["posix", "stm32", "esp32"],
    }
    default_options = {
        "shared": False,
        "platform": "posix",
    }

    exports_sources = (
        "CMakeLists.txt",
        "config.cmake",
        "include/*",
        "src/*",
    )

    no_copy_source = True

    # def configure(self):
    #     if self.options.platform == "stm32" and not self.settings.os == "baremetal" and not self.settings.arch.startswith("arm"):
    #         raise ConanInvalidConfiguration("Shared lib is not supported")
    #
    #     if self.options.platform == "stm32" and self.options.shared == True:
    #         raise ConanInvalidConfiguration("Shared lib is not supported")

    def layout(self):
        cmake_layout(self)


    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_TRY_COMPILE_TARGET_TYPE"] = "STATIC_LIBRARY"
        tc.variables["IO_LIB"] = "io-lib"
        tc.variables["IO_PLATFORM"] = str(self.options.platform)
        tc.variables["BUILD_SHARED_LIBS"] = bool(self.options.shared)
        tc.generate()


    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()


    def package(self):
        cmake = CMake(self)
        cmake.install()


    def package_info(self):
        self.cpp_info.libs = ["io-lib"]
        self.cpp_info.set_property("cmake_file_name", "io-lib")
        self.cpp_info.set_property("cmake_target_name", "io-lib::io-lib")

        platform_defines = {
            "posix": "IO_PLATFORM_POSIX",
            "stm32": "IO_PLATFORM_STM32",
            "esp32": "IO_PLATFORM_ESP32",
        }
        self.cpp_info.defines = [platform_defines[str(self.options.platform)]]

