# arduino-simpleui
SimpleUI extends graphics primitives provided by the MCUFRIEND_kbv library (based on Adafruit's GFX library) for arduino by more complex graphics elements, allowing to sort those in layers and handling events (e.g. clicks based on the Touchscreen library), making it simple to create your own UI for arduino touchscreens.

## The SimpleUI instance
### Creating a new instance
In order to create a new UI a new SimpleUI instance must be created:

```SimpleUI ui = SimpleUI(uint_16t backgroundColor);```

The SimpleUI constructor expects one argument of the type uint_16t containing the background-color on which to draw on.

### Adding/creating a new layer
SimpleUI organizes its UI elements in layers (of the class SimpleUILayer). To add/create a layer to your SimpleUI instance run:

```SimpleUILayer* layer = ui.createLayer(unsigned char z-index);```

Note: z-index must be a number higher than the number of layers existing (or an index which has not yet been used to create a layer)

### Drawing your ui to the screen
After you have succesfully created a SimpleUI instance containing the layers of the UI which then contain the UI elements you can draw it using:

```ui.draw();```

### Clearing the screen
After drawing a UI you might want to clear the screen (and the UI data stored in memory).

```ui.clearScreen();```

### Setting up SimpleUI's event managment
If your application should be able to handle clicks, you will need to set up event managment. This is an automated process and will call the corresponding SimpleUIEventListener of the clicked elements. To make sure the event managment works, you will need to call SimpleUI's listen method periodically (place it in your loop method):

```ui.listen();```

### Other useful functions
#### ```countLayers()```
Returns the amount of layers that exist at the moment.
#### ```relativeWidth(unsigned char percentage)``` and ```relativeHeight(unsigned char percentage)```
Return the absolute width corresponding to a certain percentage of the whole screen.

Note: To get the width or height compared to another element you can use the following syntax:

```ui.relativeWidth(SimpleUIObject* reference, unsigned char percentage);```

## Handling layers
### Adding UI elements
In order to add a UI element to a layer use:

```layer.addChild(SimpleUIObject* object);```

### Getting the contained UI elements
```Array<SimpleUIObject*, LAYER_MAX_SIZE> elements = layer.getChildren();```

## UI elements
### Texts
To create a SimpleUIText instance use this syntax (use "new" to create a pointer as you will need to pass pointers to your layer's ```addChild()``` method):

```SimpleUIText* text = new SimpleUIText(String text, unsigned short x, unsigned short y, uint16_t fontColor, unsigned char fontSize);```

### Buttons
The button constructor expects this syntax:

```SimpleUIButton* button = new SimpleUIButton(String text, uint16_t backgroundColor, uint16_t borderColor, uint16_t fontColor, unsigned char fontSize, unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char padding);```

### Tables
To create a table instance use:

```SimpleUITable* table = new SimpleUITable(unsigned char x, unsigned char y, unsigned char rows, unsigned char lines, unsigned char width, unsigned char height, uint16_t backgroundColor, uint16_t borderColor, uint16_t fontColor);```

### Shared properties and methods
#### Adding event listeners
To process events that are called on specific elements you can use something called "SimpleUIEventListener" (explained later). To add one to your element call:

```uiobject.addEventListener(SimpleUIEventListener* listener);```

#### Get a UI elements area
Use:

```uiobject.getArea();```

### Custom UI elements
SimpleUI is extendable by custom UI elements, as long as they provide a working ```draw()``` method which will be called when SimpleUI tries to draw your custom UI element to the screen. To create a custom UI element make it publicly inheriting ```SimpleUIObject```. In order for the event managment system to work it also needs to overwrite the following properties:

left: pixels from left

top: pixels from the top

wdth: the element's width

hght: the element's height

## SimpleUIEventListeners
To process events that are called on specific elements you can use "SimpleUIEventListener".

### Click listeners
To instantiate a click listener use:

```SimpleUIClickListener(bool (*callback)(unsigned char x, unsigned char y), SimpleUIDisplayArea area);```

Then add this listener to the element you want to listen on as described above.
