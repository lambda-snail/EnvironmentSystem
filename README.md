# EnvironmentSystem
 A simple system with a day-night cycle and customizable weather effects. 

This repository is mostly intended for my own use, to share the code between my different projects. The environment system is a conversion and slight refactor of the blueprint system featured in the great course [One Course Solution for Dynamic Sky System](https://www.udemy.com/course/unreal-engine-5one-course-solution-for-sky-weather-system/) by Vince Petrelli on Udemy.

# Usage
1. Remove all light and atmosphere things from the map.
2. Drag the `EnvironmentController` into the map

## Common Problems
Sometimes the shaders bug out and you can see the sky sphere in the background. This is usually accompanied by the moon or sun looking strange.

To fix this usually one of these things will work:
- Reload the blueprint asset
- Restart the editor
- Remove `EnvironmentController` and add it back to the map

# Future Updates

 - [ ] Add tickable day/night cycle with time awareness and configurable speed
 - [ ] Add delegates for subscribing to events such as when when the sun rises or sets, or when the weather changes 
 - [ ] Add subsystem to query the current time and manipulate the weather system in the current level
 - [ ] Bug fixes
 - [ ] Add more weather types
