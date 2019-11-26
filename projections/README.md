
GLFW dependency is setup via conan

# conan setup
if you don't have conan install it.  Otherwise From powershell,
pip install --upgrade conan

see if you have bincrafters repository: conan remote list
if you don't have bincrafters bintray repository: conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan 
see if it's there:  conan search glfw -r=public-conan



# to build using conan
Install the plugin into visual studio.
Check Tools has a Conan Package Management option
Check the path to conan (from powershell (get-command conan).Path)




