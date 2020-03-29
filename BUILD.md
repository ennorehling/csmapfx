Building CSMapFX



The project file is for Visual Studio 2008

It's 11 years behind my version, but an import seems to have worked.

disable minimal rebuilds (/Gm), deprecated feature.

Build  complains about missing tortoisegit

Looks like there is a pre-build step that generates a version.h file. disabling it lets me build.

First build complains primarily about missing header files: fx.h, ruby.h, boost/random.hpp, boost/signals/detail/*.hpp

It requires the Fox toolkit.

Lastest stable release (1.6.x) of fox was for windoes xp and VC 5

The unstable 1.7.x download seems to support later VC versions, and has a .sln solution file.

It requires ruby

Phygon said it was probably ruby 1.9

where can I get a windows build of that?

It requires boost

There is a boost folder, but it contains only a few headers from boost/signals. 

Compilation is missing random.hpp, which isn't here.

Which version of boost, though?

boost/signals was replaced by boost/signals2 a while ago, so this is definitely an old version.

Let's try the latest boost for windows, first.

At least this project should still be active.

boost.org downloads return a page that just says "Forbidden!"

their downloads are hosted on bintray.org, a paid hosting service that gives me a 403 error for everything.

Found an old SF project hosting binaries: https://sourceforge.net/projects/boost/files/boost-binaries/

last release is 1.72 (dec 2019), which seems current?

binaries exist for msvc-14.2 (64 and 32 bit).

seems boost has moved to github: https://github.com/boostorg/wiki/wiki/Getting-Started%3A-Overview

found a download, but version 1.72 no longer includes boost::signals, only signals2. cannot use this.

Let's try to find an older version

I found https://netcologne.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.7z which has a signals/connection.cpp file that looks like the one included in the CsMap archive. But these are sources, I want binaries and headers.

Maybe https://sourceforge.net/projects/boost/files/boost-binaries/ is what I need? getting the 1.66 version from here.

copying the headers from  boost_1_66_0\boost to the csmapfx boost folder to see if it will compile. 

also the random and signals subdirectories.

<boost/random.hpp> is not found, because include directories don't include the project root, adding it to the vs project.

missing boost/config/user.hpp, copy the config directory

signals needs type_traits, copying that, too.

Cannot open include file: 'boost/detail/workaround.hpp'

Cannot open include file: 'boost/core/ref.hpp': No such file or directory

forget about this, I'm adding c:\local\boost166_0 to the include directories

Info: Boost.Config is older than your compiler version - probably nothing bad will happen - but you may wish to look for an update Boost version.  Define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE to suppress this message.

defining that, then. investigate later, maybe.

It seems that the boost folder in csmap contains selected files from the boost libraries, to avoid linking against boost binaries. well, that's unusual.

success: all the boost-related stuff compiles now.

Boost Signals

resolving the missing fx.h exposes even more boost headers to the build.

Warning: Boost.Signals is no longer being maintained and is now deprecated. Please switch to Boost.Signals2. To disable this warning message, define BOOST_SIGNALS_NO_DEPRECATION_WARNING.

defining that for now.

Fox Toolkit

I'm using version 1.7.65, adding the include folder of that to the project include directories.

'FXStringVal': identifier not found

still exists in the docs for FXString.h 1.67.2.1 (2004), but not in my development version (2018).

the 1.6 docs still mention it, so I'll continue with version 1.6.57 (aka stable)

1>translator.cpp1>C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.22.27905\include\complex(462,22): error C2589:  '(': illegal token on right side of '::'this is caused by boost/signal.hpp included from fxhelper.h - what's it doing including complex?

if FLOAT_MATH_FUNCTIONS is not defined, then fxdefs.h defines atan2f, which is part of c++98

still lots of warnings, but fox-related files will now compile.

vcpkg

csmap_savePNG needs 'png.h'

I prefer to install common third-party libraries through vcpkg.

cannot clone vcpkg because my git for windows and openssl are outdated.

uninstall git 1.8, install https://gitforwindows.org/ from https://gitforwindows.org/ 

libpng

vcpkg install libpng

builds and installs the 32 bit version for now

imageio.cpp gives us "use of undefined type 'png_struct_def'"

error handler was probably written for libpng12

png 1.6 manual makes it sound like i should call png_warning or png_error, not use longjmp myself?

more longjmps junk in imageio.cpp, I'm commenting out the PNG export for now, #ifdef WITH_LIBPNGruby

at this point, I am down to just three files missing ruby.h

I found ruby 2.1 binaries at https://sourceforge.net/projects/ruby-2-1-win32-binary/

since this was built with the mingw gnu compiler. the config.h in i386-mingw32\ruby doesn't work for me, and probably can't.

found https://ftp.ruby-lang.org/pub/ruby/binaries/mswin32/ which I think contains 1.9.2 binaries for the ms compiler?

that gives #error MSC version unmatch: _MSC_VER: 1200 is expected.

this library was built against MSVC 1200 (VC6), vc++ version 60, thus #define RT_VER 60

I have VS 2019 v16.2.5, with compiler version 14.21

again, creating my own ruby/config.h in csmapfx

the disctributed one had lots of stdint limits defined, which need to be removed.

also need HAVE_STRUCT_TIMESPEC

bindings.cpp uses STR2CSTR 

SO: The function STR2CSTR was removed in Ruby 1.9 

the zips for 1.8.* on ftp.ruby-lang.org have no includes direcotry in them. I don't understand their naming, but it looks like only the *-p0-i386-win32.zip have them.

google says to use StringValuePtr instead of STR2CSTR for new code?

rb_frame_last_func doesn't exist in 1.9, but is in 1.8.7 (which i have no headers for)

oh! there are headers in ruby-1.8.7-i386-mswin32/lib/ruby/1.8/i386-mswin32 !

the directory layout was changed for 1.9

problem: for this one I cannot override the config.h easily

Looks like I need to either install VC6, or port this to ruby 1.9?

version.rc(11): fatal error RC1015: cannot open include file 'afxres.h'. -- can be replaced with windows.h for non-MFC projects

w have a pragma that links us to FOXD-1.6.lib, but our fox came without any libs?

built it, added it to linker path, remove libpng, zlib, libbz2

1>LINK : fatal error LNK1104: cannot open file 'libboost_signals-vc141-mt-sgd-x32-1_66.lib'

I need a static .lib for boost signals 1.66

it's also not in https://netix.dl.sourceforge.net/project/boost/boost/1.66.0/boost_1_66_0.7z :-(

do i need to build it myself?

start MSVC developer command prompt

run bootstrap.bat msvc

b2 to build, which takes forever, and throws several internal compiler errors

it builds 64 bit libraries by default! hours and hours of waiting for nothing.

http://informilabs.com/building-boost-32-bit-and-64-bit-libraries-on-windows/

adding address-model=32 might help?

I found precompiled boost libraries!

https://boost.teeks99.com/bin/1.66.0/boost_1_66_0-msvc-14.1-32.exe

the linker still insists that the library should be named libboost_signals-vc141-mt-sgd-x32-1_66.lib but mine is called boost_signals-vc141-mt-gd-x32-1_66.lib (and comes with a dll). 

if I rename it, I get lots of missing symbol errors, and what looks like dueling runtimes.

fxwin.h is full of pragma junk, like this:

#pragma comment(linker, "/nodefaultlib:libcmt")
