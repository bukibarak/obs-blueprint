# OBS Blueprint
Blueprint visual scripting plugin for OBS Studio !

It's distributed under the GNU General Public License v2 (or any later version) - see the accompanying LICENSE file for more details.

## How to build OBS Blueprint?

**Please note:** For now, OBS Blueprint is only compatible with Windows x64.

1. Clone the OBS Studio repository: https://github.com/obsproject/obs-studio/ 
2. Go to the `plugins` folder and clone this repository
3. Open `CMakeLists.txt` in the <ins>plugins folder</ins> (not the one inside obs-blueprint) and add this line IN ALPHABETICAL ORDER: `add_obs_plugin(obs-blueprint)` *(it should be before obs-ffmpeg)*
4. Follow https://github.com/obsproject/obs-studio/wiki/build-instructions-for-windows to generate the Visual Studio solution files
5. Open `build_x64\obs-studio.sln` with Microsoft Visual Studio or Jetbrains Rider (or any other IDE that support SLN solutions)
6. Open the `obs-blueprint` project properties and add `/EHsc` to the additional command line arguments.
    * With Rider, do the following:
        1. Right-click on `obs-blueprint` solution
        2. Choose `Properties...`
        3. Select the desired configuration (Debug, MinSizeRel, Release, RelWithDebInfo) on the left
        4. Go to `C/C++` tab
        5. At the very end of the list, add `/EHsc` at the end of the field `Command line - additional arguments`. It should look something like: `%(AdditionalOptions) /utf-8 /Brepro /Zc:__cplusplus /EHsc`
7. You can now build and run OBS studio with OBS Blueprint plugin attached!

### Authors

Made by Cédric Mujynya (Bukibarak).