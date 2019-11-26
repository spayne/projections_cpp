
GLFW dependency is setup via conan

# conan setup
if you don't have conan install it from powershell: 
pip install conan

Otherwise From powershell,
pip install --upgrade conan

see if you have bincrafters repository: conan remote list
if you don't have bincrafters bintray repository: conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan 

see if it's there:  conan search glfw -r=bincrafters
expect to see a list of 3 package recipes


# to build using conan - add it to visual studio
Install the conan plugin into visual studio. 
Tools/Extensions and Updates
pick online, search for conan
download conan extensionfor visual studio (1.2.2.346)
shutdown visual studio
select 'modify from popup dialog box'


# to build using conan - add it to visual studio
Open projections.sln 
Select conan from output window dialog box to see conan output
Choose tools/conan package management/install (current project)

Observe glfw should be installed.  

Build project should build without error.
Run project. should run without error.




