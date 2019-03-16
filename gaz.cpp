#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <cstdlib>
#include <ctime>

class V2
{
	private:
		double v[2];
	public:
		V2(){v[0] = 0; v[1] = 0;}
		V2(double x, double y){ v[0] = x; v[1] = y; }

		operator bool() const {return v[0] || v[1];}
		double& x() {return v[0];}
		double& y() {return v[1];}

		V2 operator+(const V2& other)
		{return V2(v[0]+other.v[0], v[1]+other.v[1]);}
};


class Particle
{
		V2 c, v;
	public:
		Particle(){}
		Particle(double x, double y, double vx, double vy);
		double& x(){return c.x();}
		double& y(){return c.y();}
		double& vx(){return v.x();}
		double& vy(){return v.y();}
		void advance()
		{
			c = c+v;
		}
};


class GazWidget : public QWidget
{
		Particle parts[100];
	public:
		GazWidget(QWidget* parent = 0): QWidget(parent)
		{
			setFixedSize(800,600);

			for(size_t i = 0; i < 100; ++i)
			{
				Particle& p = parts[i];
				p.x() = rand()%600 + 100;
				p.y() = rand()%400 + 100;
				p.vx()  = rand()%10 - 5;
				p.vy()  = rand()%10 - 5;
			}

			startTimer(50); // will call timerEvent each 50 ms
		}

		void timerEvent(QTimerEvent*)
		{
			for(size_t i = 0; i < 100; ++i)
			{
				Particle& p = parts[i];
				if(p.x() < 0 || p.x() > 800) p.vx() *= -1;
				if(p.y() < 0 || p.y() > 600) p.vy() *= -1;

				p.advance();
			}
			update(); // repaint window
		}

		void paintEvent(QPaintEvent*)
		{
			QPainter painter(this);
			for(size_t i = 0; i < 100; ++i)
			{
				Particle& p = parts[i];
				painter.save(); // save current coord system
				painter.translate(p.x(), p.y()); // move coord system (0,0) -> (x,y)
				painter.setBrush(Qt::green);
				painter.drawEllipse(-5,-5,10,10);
				painter.restore(); // restore coord system
		  }	
		}

};


int main(int ac, char* av[])
{
	srand(time(0));
	QApplication a(ac,av);

	GazWidget gw(0);
	gw.show();

	QObject::connect(
			&a,SIGNAL(lastWindowClosed()),
			&a,SLOT(quit()));

	return a.exec();
}

