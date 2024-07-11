
from conan import ConanFile
from conan.tools.meson import MesonToolchain, Meson
from conan.tools.layout import basic_layout

class piperifleConan(ConanFile):

    # project
    name = "piperifle"
    version = "0.0"
    description = "Data Plumbing Library"
    license = "MIT"
    url = "https://github.com/tylerh111/piperifle"
    package_type = "library"


    # configurations
    settings = [
        "os",
        "compiler",
        "build_type",
        "arch",
    ]
    generators = [
        # "MesonToolchain", # included by default
        "PkgConfigDeps",
    ]
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
    }
    exports_sources = [
        "meson.build",
        "src/*",
    ]

    # dependencies
    requires = [
        "boost/1.85.0",
    ]
    build_requires = [
        "meson/1.4.1",
    ]


    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        basic_layout(self)

    def generate(self):
        tc = MesonToolchain(self)
        tc.generate()

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        meson = Meson(self)
        meson.install()

    def package_info(self):
        self.cpp_info.libs = ["piperifle"]
