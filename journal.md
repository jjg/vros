# Journal

## 09082020

Initial entry.

Reviving this project almost 30 years after inception. 

## 09092020

Cyberspace is a generic term for non-physical spaces, typically created using electronic communications systems.

A cyberdeck is a specialized piece of computer hardware that provides the physical interface between a person and cyberspace.

The cyberdecks' combination of hardware and software renders a virtual world described by data stored within the cyberdexk itself or as data received from nodesattached to network.

The objects in this environment are created by users directly via the cyberdeck software, VROS. These objects can be created and persisted within the cyberdeck itself (local node), inside a remode node or within a Netspace of nodes where they are managed by the VNS. 

Netspace is a generic term for the cyberspace that contains a network of nodes, akin to what Gibson called "The Matrix".

Virtual Node Service or VNS (pronounced "venus"), are servers that provide a netspace around nodes on a network.  VNS servers catalog node attributes (names, addresses, etc.) and metadata and provide a shared navigatable space around groups of network nodes.  VNS provides the "outside" netspace around groups of network nodes and are responsible for communicating the position of netizens to eachother within this space.


## 09102020

Woke-up thinking about architecture this morning.  Came up with a few ideas that make the implementation seem a lot simpler.

Minimally each node maintains a list of objects that the node manages.  In the case of a cyberdeck, this includes the objects that are stored in the deck's local memory (the local node), including the user (if present), or other users (if allowed in).  The same is true for VNS nodes (whose list contains the nodes in their Netspace as well as cyberdeck users in the same space) and general-purpose "server"-type nodes (machines connected to the network dedicated to providing some service, but without a cyberdeck-style physical interface).

One of the first things a 'deck does after hardware initialization is load this list from storage and begin the display rendering thread.  This thread connects to the display device and then renders the contents of the list in a three-dimentional plane from the perspective of each "lens".

The display device is also an item in this list, and its location and orientation are stored as an entry in the list.  All input/output devices have a location stored in this list, and their visual presence is rendered in turn just like the display device.

The display thread loops through this process once at a rate set by the framerate of the display.  I think the target for this should be 60fps for now, but testing will help determine the ideal number.

Exactly what each entry in this list looks like will be somewhat flexible, and some display devices will choose to render more than others.  But here's what I think the minimum entry looks like for now:

* id
* x,y,z coordinates

With only these attributes the display device can render the object in the virtual space by applying some default values.  The result is a 1 unit sphere of a neutral gray color whose center is at the x,y,z point.

Other than the display loop, everything else is event-driven.  For example, once a dataglove input device is initialized and rendered at its initial position, it remains static until the device provides additional input.  Once the user moves their hand, the device provides input data that trigger's the virtual object's event handler connected to the input stream.  The default action for this input is to update the object's position in the list (I need to give that list a name...).  On the next iteration of the display loop, the user's hand will be redrawn in the new position.

This is how everything outside of the display loop happens.  An object is created with the ability to receive input, and when input is presented an event handler inside the object executes and updates the state of the object in the list.  I'm considering granting all objects one default sense: *collision* and one default handler: *move in response to collision*.  This way any object created will minimally move out of the way when it comes in contact with another moving object, and provide a "template" for adding additional senses and responses.

This model of connecting input and output to objects, and connecting objects to one-another is akin to the Unix philosopy of "everything is a file" and the ability to chain commands together using pipes.  My goal is to design a system that is as simple and generic as possible, and Unix is proof that this is a suitable architecture for building very complex and sophisticated things.  So I think I'm on the right track.

Let's talk a little bit more about Netspace.

When a user boots their cyberdeck they appear within the 'decks own internal cyberspace.  The local list loads, the display renders the list and the user can move around inside this space relative to the objects that they or the operating system has created there.  If the 'deck is attached to a network, the user can lave this space and venture into their local Netspace.

Physically, the cyberdeck represents one physical node on a physical network (WiFi, Ethernet, etc.).  When the user leaves the 'deck's internal space, they connect to another computer serving as a VNS, and the rendering loop switches from reading the 'deck's list to reading the list provided by the VNS.  The VNS's list contains objects representing all of the other nodes in the local Netspace, as well as any other cyberdeck users currently occupying the same space.  When the user enters another node, they once again switch lists, and start reading and writing the list for that local node.

Physically, existing inside the cyberdeck, in the local Netspace or inside another node are all pretty much the same, it's just a matter of which object list the user is interacting with.  Logically however these are distinct virtual spaces whose rules and contents are established by whoever controls the space.  The contents of each node are defined by whoever owns the node, but the Netspace around them may be managed differently.

As described above the local Netspace is just another computer with a list of objects.  The local Netspace is distinct from others in that it's the Netspace in which the user's cyberdeck exists.  There is a constellation of these spaces, provided by any number of other users, and they are connected together in a voluentary federation which allows a user to travel from one Netspace to any other.

How these spaces are created, and how they are operated is completely the domain of whoever operates them.  This could be an individual person, a corporation, a cult, a co-op, anything.  So long as the VNS complies with the established communication protocols anything beyond that is up to the operator. 

That being the case, users are encouraged to join a Netspace that meets their needs.  This may be physical proximity to the VNS server, but more likely it will be a matter of cost, philosopy, etc.

Things I need to write more about:

* A name for the object list
* How the object list should get distributed, updated, etc.
* Details about the VNS protocol, software, etc.
* Authoring and "programming" objects

## 09122020

Major system components:

* Kernel
* Networking
* Renderer
* VNS
* I/O device drivers

Future hardware architecture

Each I/O device is connected to a dedicated processor (via two-pair ethernet?) which can read and write directly to a shared block of memory.  This is very fast and maybeliminate the need for task scheduling by the kernal (since the tasks are manifested physically).

## 09122020

Began experimenting with writing graphics code against the framebuffer device in C.  Mostly a lesson in remembering C programming.


## 09132020

Making progress on the graphics programming.  I'm able to initialize the framebuffer and draw some primative shapes.

Trying to implement double buffering but creating segfaults instead.


## 09142020

Figured out that the mmap() call to use "panning" of the framebuffer for double-buffering was failing.  This manifested as a segfault because I wasn't checking the return value of mmap() before using it as a pointer.  Since -1 isn't a memory location my program is allowed access to, segfault.

Now that I know where things are going wrong I can troubleshoot the proper thing.

Looks like the heart of the problem is that the panning-based double-buffering I'm trying to use isn't possible because the video driver's maximum buffer is smaller than double the size of the display.  I could use a smaller resolution, but that's not terribly useful (and may not even be an option depending on how it's formatted on the HMD's display).  

It's worth making a note to test this code on the Pinephone and see what values it's framebuffer driver returns; maybe we'll get lucky?

If not, I'm going to have to find a different way to implement the double buffer.



## 09202020

Ran the code on the pinephone and got these results:

finfo.smem_len: 4147200
screensize:     4147200
vinfo.xres:     720
vinfo.yres:     1140

Looks like the reported video memory is the same as a single screen worth of pixels, so I don't think we're going to be able to use the panning trick here.  I'm going to see about running a more stripped-down distro on the phone and see if that changes anything, but I expect we're going to have to find another way to implement the double-buffer.

OK, making some progress with the pinephone!

Running the test code with the GUI up does nothing, however I finally found a way to get out of the GUI from the console:

    sudo init 3

This makes the screen go black and drops my ssh connection, but I'm able to reconnect and resume my tmux session.  If I run the framebuffer code now, it works!

This is good news, because it means I probably won't need to waste time looking for another build/distro just to continue these experiments on the pinephone.

There's a lot of things we could do next...

* Work on a double-buffering solution
* Work on some 3D primatives
* Get input from the accelerometer/gyro
* ???

Right now I want to focus on the graphics/VR I/O aspects as opposed to the Netspace/VNS architecture stuff since I could complete a more interesting demo without the Netspace working.  Plus I could test using the pinephone with a cheap HMD to see how that's going to work out.

Did a little research on getting accel/gyro input but it's not clear to me if this is avaliable via a standard Unix interface (/dev/*something*) or if I have to talk to the chip via whatever bus it's attached to (i2c, etc.).  I [posted a question](https://forum.pine64.org/showthread.php?tid=11554) to the Pine64 forum to see if anyone has any advice here.  While I wait to hear anything back I might switch-gears and work on some more graphics stuff.

Let's try running this on the Pinebook Pro and see what happens...

Runnning it from X does nothing, but switching to one of the virtual consoles (`ctrl-alt-F3`, then `ctrl-alt-F2` to get back) and then running the code draws the expected circle, so I think we can debug on the laptop where it's easier than the phone (for now).

Let's do something to put double-buffering to bed for now.

An initial RAM-based double-buffering implementation is in place.  It appears to work, but something weird is happening that results in the image looking "scaled-up".  I'm sure I'm just doing something dumb, but since that's not the focus at the moment I'm going to let it be for now.

Attempted to do some animation by drawing a circle in several places and clearing the screen in-between.  It is not fast.


