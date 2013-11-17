#ifndef MOBILESYNTHVIEWCONTROLLERRC1_HPP
#define MOBILESYNTHVIEWCONTROLLERRC1_HPP

namespace mobilesynthview
{
  struct WidgetImpl;
  class Widget
  {
    WidgetImpl* impl;
  public:
    Widget();
    ~Widget();
    void noteOn(int f);
    void noteOff(int f);
  };
}

#endif // MOBILESYNTHVIEWCONTROLLERRC1_HPP
