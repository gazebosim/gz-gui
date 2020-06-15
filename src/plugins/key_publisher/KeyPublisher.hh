#ifndef IGNITION_GUI_HELLOPLUGIN_HH_
#define IGNITION_GUI_HELLOPLUGIN_HH_

#include <ignition/gui/qt.h>
#include <ignition/gui/Plugin.hh>
#include <ignition/transport/Node.hh>


namespace ignition
{
  namespace gui
  {
    class KeyPublisher : public ignition::gui::Plugin  //inherited from Qobject
    {
        Q_OBJECT

        public: KeyPublisher();    //constructor
        public: virtual ~KeyPublisher();   //destructor
        
        public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;
        protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

        //Publish message
        public:
          ignition::transport::Node node;
          std::string topic = "/gazebo/keyboard/keypress";
          ignition::transport::Node::Publisher pub ;

          void keyloop(QKeyEvent *key_press);
    };
  }
}

#endif
