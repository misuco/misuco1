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
    void noteOn(int n, float f);
    void noteOff(int n);
    void pc(int n);
  };
}

#endif // MOBILESYNTHVIEWCONTROLLERRC1_HPP
