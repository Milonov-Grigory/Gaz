#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <cstdlib>
#include <ctime>
#include <list>
#include <iostream>

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
		~Particle()
		{ 
			std::cout 
				<< "Particle d-tor called!"
				<< "( " << x() << " , " << y() << ")"
				<< std::endl;
		}
		//Particle(double x, double y, double vx, double vy);
		double& x(){return c.x();}
		double& y(){return c.y();}
		double& vx(){return v.x();}
		double& vy(){return v.y();}
		void advance()
		{
			c = c+v;
		}
		void* operator new (size_t);
		void operator delete(void*);
};

struct Chunk
{
	Chunk *next;
	char buffer[sizeof(Particle)];
};

Chunk pool[100];
Chunk *head = pool;

void init_pool()
{
	for(Chunk *ptr = pool; ptr != pool+100; ++ptr)
		ptr->next = ptr+1;

	pool[99].next = NULL;
}

void* allocate()
{
	if (!head)
	{
		throw std::bad_alloc();
	}
	else	
	{
		void* res = &(head->buffer);
		head = head->next;
		return res;
	}
}

void deallocate(void* ptr)
{
	Chunk* chunk_ptr = reinterpret_cast<Chunk*>((char*)ptr - offsetof(Chunk,buffer));
	chunk_ptr->next = head;
	head = chunk_ptr;
}

void* Particle::operator new(size_t)
{
	return allocate();
}

void Particle::operator delete(void* ptr)
{
	deallocate(ptr);
}

class GazWidget : public QWidget
{
	std::list<Particle*> parts;
	void makeNewParticle()
	{
		try
		{
				Particle* p = new Particle;
				p->x() = rand()%600 + 100;
				p->y() = rand()%400 + 100;
				p->vx()  = rand()%10 - 5;
				p->vy()  = rand()%10 - 5;
				parts.push_back(p);
		}
		catch(std::bad_alloc&)
		{
		}
	}
	public:
		GazWidget(QWidget* parent = 0): QWidget(parent)
		{
			setFixedSize(800,600);
			for(size_t i = 0; i < 100; ++i)
			{
				makeNewParticle();
			}

			startTimer(50); // will call timerEvent each 50 ms
		}

		void timerEvent(QTimerEvent*)
		{
			for(std::list<Particle*>::iterator it = parts.begin();
					it != parts.end(); ++it)
			{
				Particle* p = *it;
				if(p->x() > 800 && p->y() > 200 && p->y() < 400)
				{
					parts.erase(it++);
					delete p;
					continue;
				}
				if(p->x() < 0 || p->x() > 800 ) p->vx() *= -1;
				if(p->y() < 0 || p->y() > 600 ) p->vy() *= -1;

				p->advance();
			}
			update(); // repaint window
		}

		void keyPressEvent(QKeyEvent* e)
		{
			if( e->key() == Qt::Key_Space )
			{
				makeNewParticle();
			}
		}

		void mousePressEvent(QMouseEvent* e)
		{
			try
			{
				Particle* p = new Particle;
				p->x() = e->x();
				p->y() = e->y();
				p->vx()  = rand()%10 - 5;
				p->vy()  = rand()%10 - 5;
				parts.push_back(p);
			}
			catch(std::bad_alloc&)
			{
			}

		}

		void paintEvent(QPaintEvent*)
		{
			QPainter painter(this);
			for(std::list<Particle*>::iterator it = parts.begin(); 
					it != parts.end(); ++it)
			{
				Particle* p = *it;
				painter.save(); // save current coord system
				painter.translate(p->x(), p->y()); // move coord system (0,0) -> (x,y)
				painter.setBrush(Qt::green);
				painter.drawEllipse(-5,-5,10,10);
				painter.restore(); // restore coord system
		  }	

			painter.translate(width()/2, height()/2);
			QFont current_font = font();
			current_font.setPixelSize(20);
			painter.setFont(current_font);
			painter.setPen(Qt::black);
			painter.drawText(0,0,QString("%1").arg(parts.size()));
		}

};


int main(int ac, char* av[])
{
  init_pool();
	srand(time(0));
	QApplication a(ac,av);

	GazWidget gw(0);
	gw.show();

	QObject::connect(
			&a,SIGNAL(lastWindowClosed()),
			&a,SLOT(quit()));

	return a.exec();
}

