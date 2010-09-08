
#include <xge/xge.h>
#include <xge/graph.h>
#include <xge/clock.h>
#include <xge/engine.h>

#include <tinyxml/tinyxml.h>




/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class Svg
{

protected:

	bool bFull;

	//configuration
	int      bezier_points;
	int      circle_points;
	float  tolerance;
	std::vector<float> bezier_coefficients;

	//run time
	float x,y;
	int   close_index;
	float  last_cx,last_cy;
	const char*  cursor;

	//****************************
	//tessellator
	//****************************
	EngineTessellator::polygon_t        polygon; 
	EngineTessellator::contour_t        contour; 
	EngineTessellator::polygon_set_t    polygons; 

	//ggraph
	SmartPointer<Graph> gout;

	//jump spaces
	inline void jump_spaces()
	{
		for (int ch=*cursor;*cursor && (ch==10 || ch==13 || ch=='\t' || ch==' ' || ch==',');ch=*(++cursor))	;
	}

	//opcode
	inline int popOpCode()
	{
		//[A-Za-z]
		DebugAssert ((*cursor>='A' && *cursor<='Z') || (*cursor>='a' && *cursor<='z'));
		int ret=*cursor++;
		jump_spaces();
		return ret;
	}

	//pop float
	inline float popFloat()
	{
		//(-?[0-9]+\.?[0-9]*(?:e-?[0-9]*)?"
		const char* start=cursor;
		int ch=*cursor;

		if (ch=='-' || ch=='+') ch=*(++cursor);//first sign
		
		for (ch=*cursor;ch>='0' && ch<='9';ch=*(++cursor)) 
			; //numbers on the left

		if (ch=='.') //point
		{
			ch=*(++cursor);
			for (ch=*cursor;ch>='0' && ch<='9';ch=*(++cursor)) ; //numbers on the right

			if (ch=='e' || ch=='E') 
			{
				ch=*(++cursor);//e
				if (ch=='-' || ch=='+') ch=*(++cursor);//first sign
				for (ch=*cursor;ch>='0' && ch<='9';ch=*(++cursor)) ; 
			}
		}
			
		float ret=(float)atof(start);
		jump_spaces();
		return ret;
	}

    // set_position
    inline void set_position(float x,float y)
	{
        this->x = x;
        this->y = y;
		this->contour.push_back(Vec3f(x,y,0));
	}

	// line_to
    inline void  line_to(float x,float y)
	{
        this->set_position(x, y);
	}


	//new polygon
	inline void newPolygon()
	{
		this->polygon.clear();
	}

	//end polygon
	inline void endPolygon()
	{
		// final case
        if (this->polygon.size())
			polygons.push_back(this->polygon);			
        
		//clear the contour
        this->polygon.clear();
	}

    // newContour
    inline void newContour()
	{
        this->x = 0;
        this->y = 0;
        this->close_index = 0;
        this->contour.clear();
	}

    // endContour
    inline void endContour()
	{
		if (this->contour.size())
		{
			// a little filter for paths
			//TODO: also for full?
			if (!bFull)
			{
				const float filter_tolerance=0.1f;
				EngineTessellator::contour_t filtered;
				Vec3f last=this->contour[0];
				filtered.push_back(last);

				for (int I=0;I<(int)this->contour.size();I++)
				{
					if ((contour[I]-last).module()>filter_tolerance)
					{
						filtered.push_back(contour[I]);
						last=contour[I];
					}
				}

				contour=filtered;
				DebugAssert(contour.size());
			}
			this->polygon.push_back(this->contour);
		}
		this->contour.clear();
	}

    // closeContour
    inline void closeContour()
	{
        this->contour.push_back(this->contour[this->close_index]);
        this->close_index = (int)this->contour.size();
	}

    // local function definition
    float angle(float u[2], float v[2])
	{
        float a = (float)acos((u[0]*v[0] + u[1]*v[1]) / (float)sqrt((u[0]*u[0] + u[1]*u[1]) * (v[0]*v[0] + v[1]*v[1])));
		float sgn = (u[0]*v[1] > u[1]*v[0])?1.0f:-1.0f;
        return sgn * a;
	}

    // arc_to
    inline void arc_to(float rx, float ry, float phi, float large_arc, float sweep, float x, float y)
	{
        // This function is made out of magical fairy dust
        // http://www.w3.org/TR/2003/REC-SVG11-20030114/implnote.html//ArcImplementationNotes
        float x1 = this->x;
        float y1 = this->y;
        float x2 = x;
        float y2 = y;
        float cp = (float)cos(phi);
        float sp = (float)sin(phi);
        float dx = 0.5f * (x1 - x2);
        float dy = 0.5f * (y1 - y2);
        float x_ = cp * dx + sp * dy;
        float y_ = -sp * dx + cp * dy;
        float r = (float)sqrt((pow((rx * ry),2) - pow((rx * y_),2) - pow((ry * x_),2))/(pow((rx * y_),2) + pow((ry * x_),2)));
        
        if (large_arc != sweep)
			r = -r;

        float cx_ = r * rx * y_ / ry;
        float cy_ = -r * ry * x_ / rx;
        float cx = cp * cx_ - sp * cy_ + 0.5f * (x1 + x2);
        float cy = sp * cx_ + cp * cy_ + 0.5f * (y1 + y2);

		float _u1[2]={1,0};
		float _v1[2]={((x_ - cx_)/rx, (y_ - cy_)/ry)};
        float psi = angle(_u1,_v1);

		float _u2[2]={((x_ - cx_)/rx, (y_ - cy_)/ry)};
		float _v2[2]={((-x_ - cx_)/rx, (-y_ - cy_)/ry)};
        float delta = angle(_u2,_v2);

        if ( sweep && delta < 0) delta += (float)M_PI * 2;
        if (!sweep && delta > 0) delta -= (float)M_PI * 2;
        int n_points = max2(int(abs(this->circle_points * delta / (2 * M_PI))), 1);
        
        for (int i=0;i<=n_points;i++)
		{
            float theta = psi + i * delta / n_points;
            float ct = (float)cos(theta);
            float st = (float)sin(theta);
            this->line_to(cp * rx * ct - sp * ry * st + cx,sp * rx * ct + cp * ry * st + cy);
		}
	}

    // curve_to
    inline void curve_to(float x1,float y1,float x2,float y2,float x,float y)
	{    
        this->last_cx = x2;
        this->last_cy = y2;

		float px,py;

		for (int i=0;i<(int)this->bezier_coefficients.size();i+=4)
		{
            px = this->bezier_coefficients[i+0] * this->x + this->bezier_coefficients[i+1] * x1 + this->bezier_coefficients[i+2] * x2 + this->bezier_coefficients[i+3] * x;
            py = this->bezier_coefficients[i+0] * this->y + this->bezier_coefficients[i+1] * y1 + this->bezier_coefficients[i+2] * y2 + this->bezier_coefficients[i+3] * y;
			this->contour.push_back(Vec3f(px, py,0));
		}

        this->x=px;
		this->y=py;
	}

	//main parse function
	void parse(TiXmlElement* xnode)
	{
		const char* xname=xnode->Value();

		// TODO: reinclude this cases!
        if (!strcmpi(xname,"polyline") || !strcmpi(xname,"polygon") || !strcmpi(xname,"line"))
            return;
     
		//TODO TRANSFORM
		const char* transform=xnode->Attribute("transform");
		//DebugAssert (!transform);

        // ____________________________________________________________________________________________ contour
		if (!strcmpi(xname,"path"))
		{
			const char* pathdata=xnode->Attribute("d");
			this->cursor=pathdata;jump_spaces();

			newPolygon();
			newContour();

			while (*cursor)
			{
				const char opcode = popOpCode();

				if (opcode == 'M')
				{   
                    this->endContour();
                    this->newContour();
					float x=popFloat();
					float y=popFloat();
                    this->set_position(x,y);
				}  
                else if (opcode == 'C')
				{
					float x1=popFloat();
					float y1=popFloat();
					float x2=popFloat();
					float y2=popFloat();
					float x3=popFloat();
					float y3=popFloat();
                    this->curve_to(x1,y1,x2,y2,x3,y3);
				}
                else if (opcode == 'c')
				{
                    float mx = this->x;
                    float my = this->y;
					float x1=popFloat();
					float y1=popFloat();
					float x2=popFloat();
					float y2=popFloat();
					float x =popFloat();
					float y =popFloat();
                    this->curve_to(mx + x1, my + y1, mx + x2, my + y2, mx + x, my + y);
                }
                else if (opcode == 'S')
				{
					float x2=popFloat();
					float y2=popFloat();
					float x3=popFloat();
					float y3=popFloat();
                    this->curve_to(2 * this->x - this->last_cx, 2 * this->y - this->last_cy, x2,y2,x3,y3);
				}   
                else if (opcode == 's')
				{
                    float mx = this->x;
                    float my = this->y;
                    float x1= 2 * this->x - this->last_cx;
					float y1= 2 * this->y - this->last_cy;
					float x2=popFloat();
					float y2=popFloat();
					float x =popFloat();
					float y =popFloat();
                    this->curve_to(x1, y1, mx + x2, my + y2, mx + x, my + y);
				}
                else if (opcode == 'A')
				{
					float rx=popFloat();
					float ry=popFloat();
					float phi=popFloat();
					float large_arch=popFloat();
					float sweep=popFloat();
					float x=popFloat();
					float y=popFloat();
                    this->arc_to(rx, ry, phi, large_arch, sweep, x, y);
				}
                else if (opcode == 'z')
				{
                    this->closeContour();
                }    
                else if (opcode == 'L')
				{
					float x=popFloat();
					float y=popFloat();
                    this->line_to(x,y);
                }
                else if (opcode == 'l')
				{
                    float x=popFloat();
					float y=popFloat();
                    this->line_to(this->x + x, this->y + y);
                }
                else if (opcode == 'H')
				{
                    float x = popFloat();
                    this->line_to(x, this->y);
                }
                else if (opcode == 'h')
				{
                    float x = popFloat();
                    this->line_to(this->x + x, this->y);
                }
                else if (opcode == 'V')
				{
                    float y =popFloat();
                    this->line_to(this->x, y);
                } 
                else if (opcode == 'v')
				{
                    float y = popFloat();
                    this->line_to(this->x, this->y + y);
                } 
                else
				{
                    Log::printf("Warning: svg_parser Parser. unrecognised opcode: %c\n",opcode);
				}
			}

            this->endContour();
			this->endPolygon();
		}

        // ____________________________________________________________________________________________ rect
		else if (!strcmpi(xname,"rect"))
		{
            float x=(float)atof(xnode->Attribute("x"));
			float y=(float)atof(xnode->Attribute("y"));
			float w=(float)atof(xnode->Attribute("width"));
			float h=(float)atof(xnode->Attribute("height"));

			this->newPolygon();
			{
				this->newContour();
				this->set_position(x, y);
				this->line_to(x+w,y);
				this->line_to(x+w,y+h);
				this->line_to(x,y+h);
				this->line_to(x,y);
				this->endContour();
			}
			this->endPolygon();
		}

        // ____________________________________________________________________________________________ polyline,polygon
		else if (!strcmpi(xname,"polyline") || !strcmpi(xname,"polygon"))
		{
			const char* points=xnode->Attribute("points");
			this->cursor=points;jump_spaces();

			this->newPolygon();
			{
				this->newContour();
				while (*cursor)
				{
					float x=popFloat();
					float y=popFloat();
					this->line_to(x,y);
				}
				if (!strcmp(xname,"polygon"))
					this->closeContour();

				this->endContour();
			}       
			this->endPolygon();
		}
            
        // ____________________________________________________________________________________________ line
        else if (!strcmpi(xname,"line"))
		{
            float x1 = (float)atof(xnode->Attribute("x1"));
            float y1 = (float)atof(xnode->Attribute("y1"));
            float x2 = (float)atof(xnode->Attribute("x2"));
            float y2 = (float)atof(xnode->Attribute("y2"));
			this->newPolygon();
			{
				this->newContour();
				this->set_position(x1, y1);
				this->line_to(x2, y2);
				this->endContour();
			}
			this->endPolygon();
		}
        // ____________________________________________________________________________________________ circle
        else if (!strcmpi(xname,"circle"))
		{
            float cx = (float)atof(xnode->Attribute("cx"));
            float cy = (float)atof(xnode->Attribute("cy"));
            float r  = (float)atof(xnode->Attribute("r"));

			this->newPolygon();
			{
				this->newContour();
				for (int i=0;i<this->circle_points;i++)
				{
					float theta = 2 * i * (float)M_PI / (float)this->circle_points;
					this->line_to(cx + r * cos(theta), cy + r * sin(theta));
				}
				this->closeContour();
				this->endContour();
			}
			this->endPolygon();
		}
            
        // ____________________________________________________________________________________________ ellipse
		else if (!strcmpi(xname,"ellipse"))
		{
            float cx = (float)atof(xnode->Attribute("cx"));
            float cy = (float)atof(xnode->Attribute("cy"));
            float rx = (float)atof(xnode->Attribute("rx"));
            float ry = (float)atof(xnode->Attribute("ry"));
			this->newPolygon();
			{
				this->newContour();
				for (int i=0;i<this->circle_points;i++)
				{
					float theta = 2 * i * (float)M_PI / (float)this->circle_points;
					this->line_to(cx + rx * cos(theta), cy + ry * sin(theta));
				}
				this->closeContour();
				this->endContour();
			}
			this->endPolygon();
		}
            

		#if 0
		//print out attributes
		for (TiXmlAttribute* att=xnode->FirstAttribute();att;att=att->Next())
		{
			const char* attribute_name =att->Name();
			const char* attribute_value=att->Value();
			Log::printf("  %s = %s\n",attribute_name,attribute_value);
		}
		#endif

		//recursive visit
		for (TiXmlNode* xchild = xnode->FirstChild(); xchild != 0; xchild = xchild->NextSibling()) 
		{
			if (xchild->Type()==TiXmlNode::ELEMENT)
				parse((TiXmlElement*)xchild);
		}
	}

public:

	//constructor
	Svg(std::string filename,bool _bFull,int _bezier_points=4, int _circle_points=8,float _tolerance= 0.001):
		bezier_points(_bezier_points),circle_points(_circle_points),tolerance(_tolerance),bFull(_bFull)
	{
		//build bezier coefficients
		for (int i=0;i<=this->bezier_points;i++)
		{
			//http://it.wikipedia.org/wiki/Curva_di_B%C3%A9zier
			float t = float(i)/this->bezier_points;
            float t0 = pow((1 - t),3);
            float t1 = 3 * t * pow( (1 - t) ,2);
            float t2 = 3 * t*t * (1 - t);
            float t3 = pow(t ,3);
            this->bezier_coefficients.push_back(t0);
			this->bezier_coefficients.push_back(t1);
			this->bezier_coefficients.push_back(t2);
			this->bezier_coefficients.push_back(t3);
		}


		Clock t1;
		Log::printf("Opening file %s\n",filename.c_str());

		unsigned long filesize;
		unsigned char* buff=FileSystem::ReadFile(filename,filesize,true);
		DebugAssert(filesize);

		Log::printf("parsing ...\n");
		TiXmlDocument doc;
		if (!doc.Parse((const char*)buff))
		{
			Log::printf("Failed of open_xml for file %s errcode=%d (%s)",filename.c_str(),doc.ErrorRow(),doc.ErrorDesc());
			MemPool::getSingleton()->free(filesize,buff);
		}
		MemPool::getSingleton()->free(filesize,buff);
		Log::printf("...done\n");

		//doc.Print();

		TiXmlElement* root=doc.RootElement();

		//first pass, only file or dir
		for (TiXmlNode* xchild = root->FirstChild(); xchild != 0; xchild = xchild->NextSibling()) 
		{
			if (xchild->Type()==TiXmlNode::ELEMENT)
				parse((TiXmlElement*)xchild);
		}

		Log::printf("...done in %d msec\n",(int)(Clock()-t1));

		//Box3f box=this->getBBox();
		//guessBestPosition(box,Vec3f(0,0,1),DEFAULT_FOV,1.0f);


		//as a full representation (==convert all polygons to full cells)
		if (bFull)
		{
			//tessallation of all polygons
			Log::printf("Tessellating...\n");

			EngineTessellator tessellator;

			for (EngineTessellator::polygon_set_t::const_iterator PT=polygons.begin();PT!=polygons.end();PT++)
			{
				const EngineTessellator::polygon_t& polygon=*PT;
				tessellator.AddPolygon();

				for (std::vector<EngineTessellator::contour_t>::const_iterator CT=polygon.begin();CT!=polygon.end();CT++)
				{
					const EngineTessellator::contour_t& contour=*CT;
					tessellator.AddContour(contour);
				}
			}

			tessellator.Run();

			ReleaseAssert(tessellator.GetMatrix().almostIdentity());

			this->gout=tessellator.GetGraph();
			Log::printf("...done\n");
		}

		//as a boundary representation (==add only lines)
		else
		{
			this->gout.reset(new Graph(2));
			
			for (EngineTessellator::polygon_set_t::const_iterator IT=polygons.begin();IT!=polygons.end();IT++)
			{
				const EngineTessellator::polygon_t& polygon=*IT;

				for (EngineTessellator::polygon_t::const_iterator JT=polygon.begin();JT!=polygon.end();JT++)
				{
					const EngineTessellator::contour_t& contour=*JT;

					//first vertex
					unsigned int V0=0;
					unsigned int C =0;

					for (int V=0;V<(int)contour.size();V++)
					{
						Vec3f _v=contour[V];
						Vecf v(1.0f,contour[V].x,contour[V].y);
						unsigned int V1=gout->addVertex(v);
						
						if (V0)
						{
							//add the edge
							if (!C) 
								C=this->gout->addNode(2);
							
							unsigned int E=gout->addEdge(V0,V1);
							gout->addArch(E,C);

							//it does not matter the orientation?
							Planef h=gout->getFittingPlane(E);
							memcpy(gout->getGeometry(E,true),h.mem,sizeof(float)*3);
						}
						V0=V1;
					}
				}
			}

			//mirroring along Y
			this->gout->scale(Vecf(0.0f,1.0f,-1.0f));
		}
	}

	SmartPointer<Graph> getGraph()
	{
		return this->gout;
	}

	//destructor
	~Svg()
	{
	
	}
};


//////////////////////////////////////////////////////////////////
SmartPointer<Graph> Graph::open_svg(std::string filename,bool bFull)
{
	if (!filename.length()) return SmartPointer<Graph>();
	return Svg(filename,bFull).getGraph();
}


