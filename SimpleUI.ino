#include <Array.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define LAYER_MAX_SIZE 4
#define LAYER_MAX_COUNT 2
#define EVENT_MAX_COUNT 2
#define EVENT_TYPE_COUNT 1
#define EVENT_CLICK 0
#define CHAR_HEIGHT 5
#define CHAR_WIDTH 5
#define TABLE_MAX_CELLS 64
#define TABLE_CONTENT_MAX_LENGTH 4
#define LIB_DIFF_WIDTH 0.255
#define LIB_DIFF_HEIGHT 0.35555
#define TABLE 1

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft(A3, A2, A1, A0, A4); 

class SimpleUIDisplayArea {
  public:
  unsigned short x;
  unsigned short y;
  unsigned short width;
  unsigned short height;
  SimpleUIDisplayArea() {};
  SimpleUIDisplayArea(unsigned short left, unsigned short top, unsigned short wdth, unsigned short hght) {
    x = left;
    y = top;
    width = wdth;
    height = hght;
  }
  bool hitsArea(unsigned short l, unsigned short t) {
    if(l > x && l < (x + width) && t > y && t < (y + height)) {
      return true;
    }
    return false;
  }
  bool touchHitsArea(unsigned short l, unsigned short t) {
    // Touchscreen and MCUFRIEND_kbv use different screen orientation, this helper method tries to fix it, allowing to pass in data received from Touchscreen lib
    return hitsArea((t * LIB_DIFF_WIDTH), (unsigned short)(340 - (l * LIB_DIFF_HEIGHT)));
  }
  unsigned short getX() {
    return x;
  }
  unsigned short getY() {
    return y;
  }
};
class SimpleUIEventListener {
  protected:
  unsigned char type;
  bool (*xcall)(unsigned char, unsigned char);
  SimpleUIDisplayArea area;
  public:
  void trigger(unsigned char x, unsigned char y) {
    xcall(x,y);
  }
  unsigned char getType() {
    return type;
  }
  SimpleUIDisplayArea getArea() {
    return area;
  }
};
class SimpleUIClickListener : public SimpleUIEventListener {
  public:
    SimpleUIClickListener(bool (*xc)(unsigned char x, unsigned char y), SimpleUIDisplayArea a) {
    type = EVENT_CLICK;
    xcall = xc;
    area = a;
  }
};
class SimpleUIObject {
  protected:
  uint16_t backgroundColor;
  uint16_t borderColor;
  unsigned char type;
  unsigned short wdth;
  unsigned short hght;
  unsigned short left;
  unsigned short top;
  unsigned char padding;
  Array<SimpleUIEventListener*, EVENT_MAX_COUNT> listeners[EVENT_TYPE_COUNT];
  public:
  uint16_t getBackgroundColor() {
    return backgroundColor;
  }
  uint16_t getBorderColor() {
    return borderColor;
  }
  unsigned char getType() {
    return type;
  }
  unsigned char getWidth() {
    return wdth;
  }
  unsigned char getHeight() {
    return hght;
  }
  SimpleUIDisplayArea getArea() {
    return SimpleUIDisplayArea(left, top, wdth, hght);
  }
  virtual void draw() {};
  void addEventListener(SimpleUIEventListener* lis) {
    listeners[lis->getType()].push_back(lis);
  }
  void clearEventListeners() {
    for (unsigned char i = 0; i < EVENT_TYPE_COUNT; i++) {
      listeners[i].clear();
    }
  }
  void click() {
    for (unsigned char i = 0; i < listeners[EVENT_CLICK].size(); i++) {
      listeners[EVENT_CLICK][i]->trigger(0,0);
    }
  }
  void click(unsigned char x, unsigned char y) {
    for (unsigned char i = 0; i < listeners[EVENT_CLICK].size(); i++) {
      listeners[EVENT_CLICK][i]->trigger(x,y);
    }
  }
};
class SimpleUITableCell : public SimpleUIObject {
  public:
  SimpleUITableCell(unsigned char w, unsigned char y, uint16_t bC, uint16_t boC) {
    backgroundColor = bC;
    borderColor = boC;
    wdth = w;
    hght = y;
  }
  void draw();
};
class SimpleUITable : public SimpleUIObject {
  protected:
  unsigned char x;
  unsigned char y;
  unsigned char type;
  uint16_t cellBorderColor;
  uint16_t fontColor;
  String * values;
  bool hasValues;
  public:
  SimpleUITable(unsigned char l, unsigned char t, unsigned char rows, unsigned char lines, unsigned char w, unsigned char h, uint16_t bC, uint16_t boC, uint16_t foC) {
    x = rows;
    y = lines;
    backgroundColor = bC;
    wdth = w;
    hght = h;
    type = TABLE;
    left = l;
    top = t;
    values = new String[rows * lines];
    hasValues = false;
    fontColor = foC;
  }
  void setValues(String * v) {
      values = v;
      hasValues = true;
  }
  unsigned char getRowCount() {
    return x;
  }
  unsigned char getLineCount() {
    return y;
  }
  void setCellBorder(uint16_t boC) {
    cellBorderColor = boC;
  }
  uint16_t getCellBorderColor() {
    return cellBorderColor;
  }
  /*SimpleUITableCell* getCell(unsigned char i) {
    return children[i];
  }*/
  void draw() {
    for (int i = 0; i < y; i++) {
      unsigned char cellXSize = wdth / x;
      unsigned char cellYSize =  hght / y;
      for(unsigned char c = 0; c < x; c++) {
        tft.fillRect(left + (c * cellXSize), top + (i * cellYSize), cellXSize, cellYSize, backgroundColor);
        tft.drawRect(left + (c * cellXSize), top + (i * cellYSize), cellXSize, cellYSize, borderColor);
        if(hasValues) {
          tft.setCursor(left + (c * cellXSize) + 2 , top + (i * cellYSize) + 2);
          tft.setTextColor(fontColor);
          tft.setTextSize(2);
          tft.print(values[i * x + c]);
        }
      }
    }
  }
  
};
class SimpleUIButton : public SimpleUIObject {
  private:
  String contentText;
  bool fixedSize;
  uint16_t fontColor;
  unsigned char fontSize;
  public:
  SimpleUIButton(String text, uint16_t c, uint16_t bC, uint16_t fC, unsigned char fS, unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned char p) {
    contentText = text;
    left = x;
    top = y;
    wdth = w;
    hght = h;
    backgroundColor = c;
    padding = p;
    fontColor = fC;
    fontSize = fS;
    borderColor = bC;
  }
  /*SimpleUIButton(String text, unsigned short x, unsigned short y) {
    
  }*/
  void draw() {
    Serial.print("drawing button...\n");
    tft.fillRect(left, top, wdth, hght, backgroundColor);
    tft.drawRect(left, top, wdth, hght, borderColor);
    tft.setCursor(left + padding, top + padding);
    tft.setTextColor(fontColor);
    tft.setTextSize(fontSize);
    tft.print(contentText);
    Serial.print("drawing button complete\n");
  }
};
class SimpleUIText : public SimpleUIObject {
  private:
  String contentText;
  uint16_t fontColor;
  unsigned char fontSize;
  public:
  SimpleUIText(String t, unsigned short x, unsigned short y, uint16_t fC, unsigned char fS) {
    contentText = t;
    fontColor = fC;
    fontSize = fS;
    left = x;
    top = y;
  }
  void draw() {
    tft.setCursor(left, top);
    tft.setTextColor(fontColor);
    tft.setTextSize(fontSize);
    tft.print(contentText);
  }
  void changeText(String t) {
    contentText = t;
  }
};
class SimpleUILayer {
  private:
  Array<SimpleUIObject*, LAYER_MAX_SIZE> children;
  unsigned char cc = 0;
  public:
  void addChild(SimpleUIObject* child) {
    children.push_back(child);
    cc++;
  }
  void drawLayer() {
    for (int i = 0; i < cc; i++) {
      children[i]->draw();
    }
  }
  Array<SimpleUIObject*, LAYER_MAX_SIZE> getChildren() {
    return children;
  }
};
class SimpleUI {
  private:
  uint16_t backgroundColor;
  unsigned short width;
  unsigned short height;
  Array<SimpleUILayer*, LAYER_MAX_COUNT> layers;
  unsigned char lc;
  public:
  SimpleUI(uint16_t bC) {
    backgroundColor = bC;
    uint16_t ID = tft.readID();
    tft.begin(ID);
    width = tft.width();
    height = tft.height();
    lc = 0;
  }
  SimpleUILayer *createLayer(unsigned char z) {
     Serial.print("creating layer...\n");
     layers.push_back(new SimpleUILayer());
     lc = z + 1;
     return layers[z];
  }
  void draw() {
    tft.fillScreen(backgroundColor);
    for(int i = 0; i < layers.size(); i++) {
      Serial.print("initializing layer...\n");
      layers[i]->drawLayer();
    }
  }
  unsigned short relativeWidth(unsigned char percentage) {
    return percentage / 100 * width;
  }
  unsigned short relativeWidth(SimpleUIObject* parent, unsigned char percentage) {
    return percentage / 100 * parent->getWidth();
  }
  unsigned short relativeHeight(unsigned char percentage) {
    return percentage / 100 * height;
  }
  unsigned short relativeHeight(SimpleUIObject* parent, unsigned char percentage) {
    return percentage / 100 * parent->getHeight();
  }
  unsigned char countLayers() {
    return lc;
  }
  void clearScreen() {
    tft.fillScreen(backgroundColor);
    layers.clear();
  }
  void listen() {
    TSPoint point = ts.getPoint();
    if (point.z > ts.pressureThreshhold) {
      for(unsigned char l = 0; l < layers.size(); l++) {
        for(unsigned char e = 0; e < layers[l]->getChildren().size(); e++) {
          if(layers[l]->getChildren()[e]->getArea().touchHitsArea(point.x, point.y)) {
            layers[l]->getChildren()[e]->click((point.x - layers[l]->getChildren()[e]->getArea().getX()), (point.y - layers[l]->getChildren()[e]->getArea().getY()));
          }
        }
      }
    }
  }
};
