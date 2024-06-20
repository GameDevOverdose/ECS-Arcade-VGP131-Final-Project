# VGP131-Final Project 👨‍💻

Hey Guys! __Ayush Vaibhav Goyal__ (GameDevOverdose) here.



# Background 🖼️

I am currently studying in LaSalle College Vancouver for a Bachelor's in Game Programming and with the end of our 3rd Quarter coming close, as our final project, we were tasked by our Object-Oriented Programming In C++ II (VGP131) Professor [Amir Jahanlou](https://github.com/AmirJahan), to create an ECS (Entity Component System) based Game Engine and make a game in it to represent the engine's capability. We were given 3 weeks for the project (though I only ended up really working on it in the last 2 weeks), and once I was done with a basic implementation of my ECS, I started brainstorming about the game that I'll be making.

I thought what would be better to represent the versatility and the modularity of the Game Engine than to make 4 different games in it! So that's what I did.


# ECS (Engine) 🚂

The architecture of the Engine is more or less how any ECS functions. There's a class for Entity that handles Entity managment and basically all objects in the game, a superclass for Component that stores the Engine's components like Transform, Sprites etc., and then a superclass for System that has several of the Engine's systems, some of which are used by calling methods, while others iterate and run automatically over an array of entities with the components that said system is supposed to deal with. So the Rendering System iterates over entities with Sprite Component (Though this isn't exactly my engine works) and renders them out, the same as something like the collision system. These 3 parts are more or less separate from each other structurally but together comprise the Game Engine.


# 🕹️ ECS Arcade (Game) 👾

The game is completely separate from the Engine and uses its methods and systems to actually make the game functional. ECS Arcade combines 4 relatively different games, mechanics and systems into a single cohesive experience, representing the versatility of the ECS engine technically and also metaphorically through its story, giving a glimpse into a slice of Video Game History, when arcade machines were no longer bound to a single video game but rather could be reprogrammed and repurposed to play something else which I find to be quite similar to ECS's impact on the gaming industry through it's extremely modularized structure that allows games of all kind to be built in a single engine in a way that's efficient, performant and simple to use. Through this project, our professor __Amir__, not only wanted us to get to grips with the ECS Paradigm but also appreciate the architecture of it, and even though I may or may not have a deeper understanding of it, I definitely do have more respect for it after this project.


## Instructions to Run 💻

- Go to the releases
- Download and Run the latest release
- Enjoy!

# Things to Improve 🎯

A project like this was a huge undertaking for us, especially in such a small time frame, so it goes without being said, that there are huge chunks of both the ECS and the Game that we feel like we could've improved in terms of the Code:

## Engine

- The Entity class is not as friendly as it should be, getting components and making changes to than should be a lot simpler.
- The Sprite System as well as the Collision System are a bit strange for a lack of a better term, and should be much more structured to be intuitive.
- Systems like Rendering and Collision iterate over vectors with the entities that have to be manually pushed by the game developer, whereas something like this should be handled automatically by the Engine. Ideally, it should iterate over all entities and then  select and store references to the ones with components it should run a system on.
- Similar to the last point, the render system renders entities that were pushed last, last (so on top). I instead wanted to change the Transform component to store Transform as transformXYZ[3] instead of just transformXY[2] so the Render System would then Render entities with higher Z values on top. It makes the System easier to use as well as more scalable.
- Should use Smart Pointers for better memory management.

## Game

- The game part of the code is arguably the messiest in the entire project, because it was the last part of the project that we worked on before submitting it. The game's files should be separated into different files.


# Game Demonstration (Gameplay) 🎮

https://github.com/GameDevOverdose/VGP131-Final-Project/assets/42678129/4b9325ad-a37d-4d92-b9ab-407d34e6dfa7

# Links 🔗

[![Itch.io](https://img.shields.io/badge/itch.io-%23FF0B34.svg?logo=Itch.io&logoColor=white)](https://gamedevoverdose.itch.io/)  [![LinkedIn](https://img.shields.io/badge/Linkedin-%230077B5.svg?logo=linkedin&logoColor=white)](https://ca.linkedin.com/in/ayush-vaibhav-goyal-aa17952b6)

