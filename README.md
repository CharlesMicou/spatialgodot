## How is the Godot Engine Different in this repo?

The `godot` directory in this project is a git subtree of a fork of the godot engine (see: [the official source](https://github.com/godotengine/godot)).
The fork comes from `115885b1bff767a905ce98ba739fad15c5925ee0`.

I've tried to keep all my work strictly within the `modules/spatialos` module. However, because I didn't want to spend ages fiddling with getting things to build nicely, I have made the following changes outside of the module:
* Deleted the x Module (because of a dependency version clash with the SpatialOS libs)
* Deleted the x Module (because of a dependency version clash with the SpatialOS libs)
* Changed the compilation settings for the windows platform build to use `MD` instead of `MT`
* Changed this README file to account for the changes I've made.
