#ifndef PANEL_H
#define PANEL_H

namespace venk {
	class Panel
	{
  private:
		std::list<Panel*> children;
		Matrix44           panelTransform;		
		std::array<GLfloat,8>   panelVertices;
		ColorRGBA          panelColor;
		std::shared_ptr<Panel>  parent;
		std::shared_ptr<Buffer> vb;		
		float x,y;
		float w,h;
public:
		Panel();
		Panel(std::shared_ptr<Panel> parent, float leftx, float topy, float width, float height);
		void set_parent(std::shared_ptr<Panel> parent);
		std::shared_ptr<Panel> get_parent();
		void calcluate_transform();
		void add_child(Panel* child);
		void remove_child(Panel* child);
		void set_color(float r, float g, float b);
		bool render();		
	};
}
#endif






