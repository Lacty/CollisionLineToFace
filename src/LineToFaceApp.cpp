
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LineToFaceApp : public AppNative {
private:
  struct Line {
    Vec3f start;
    Vec3f end;
    Color color;
  };
  Line line;
  
  struct Face {
    Vec3f v1, v2, v3;
    Vec3f normal;
    Vec3f center;
    Color color;
  };
  Face face;
  
  Vec3f intersect;
  
  
  bool isCollision(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3,
                   const Vec3f& r1, const Vec3f& r2, Vec3f& IntersectPos);
  
public:
  void setup();
  void mouseDown(MouseEvent event);
  void update();
  void draw();
};

void LineToFaceApp::setup() {
  line.start = Vec3f(0, 255, 0);
  line.end   = Vec3f::zero();
  line.color = Color(1, 0, 0);
  
  face.v1 = Vec3f(250, 100, -50);
  face.v2 = Vec3f(350, 250, 50);
  face.v3 = Vec3f(250, 400, -50);
  
  // Find Normal From Polygon
  face.normal = [&] {
    Vec3f a = face.v2 - face.v1;
    Vec3f b = face.v3 - face.v1;
    return b.cross(a).normalized();
  } ();
  
  // Find Center From Polygon
  face.center = [&] {
    return (0.33333333f * face.v1) +
           (0.33333333f * face.v2) +
           (0.33333333f * face.v3);
  } ();
  
  face.color = Color::white();
}

void LineToFaceApp::mouseDown(MouseEvent event) {}

void LineToFaceApp::update() {
  line.end = Vec3f(getMousePos(), 0);
}

void LineToFaceApp::draw() {
	// Clear out the window with black
	gl::clear(Color(0, 0, 0));
  
  if (isCollision(face.v1, face.v2, face.v3, line.start, line.end, intersect)) {
    face.color = Color(1, 0, 0);
  }
  else {
    face.color = Color(1, 1, 1);
  }
  
  gl::color(face.color);
  gl::drawLine(face.v1, face.v2);
  gl::drawLine(face.v2, face.v3);
  gl::drawLine(face.v3, face.v1);
  
  gl::drawCube(face.center, Vec3f::one());
  
  gl::color(line.color);
  gl::drawLine(line.start, line.end);
  
  //console() << intersect << endl;
}


bool LineToFaceApp::isCollision(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3,
                                const Vec3f& r1, const Vec3f& r2, Vec3f& IntersectPos)
{
  Vec3f Normal, Center, Intersect;
  
  // Find Triangle Normal
  Normal = (v3 - v1).cross(v2 - v1);
  Normal.normalize();
  
  // Find Triangle Center #1/3 = 0.33333333...
  Center = [&] {
    return (0.33333333f * v1) +
           (0.33333333f * v2) +
           (0.33333333f * v3);
  } ();
  
  // Find Distance from r1,r2 to the Plane Center
  Vec3f Dist1 = r1 - Center;
  Vec3f Dist2 = r2 - Center;
  
  // It Checks whether the line intersects the plane
  if (!(Dist1.dot(Normal) * Dist2.dot(Normal) <= 0.0f)) {
    return false;
  }
  
  // Find Vertical Distance from r1,r2 to the Plane
  float VerticalDist1 = [&] {
    return abs(Normal.dot(Vec3f(r1 - v1)));
  } ();
  
  float VerticalDist2 = [&] {
    return abs(Normal.dot(Vec3f(r2 - v1)));
  } ();

  // Find Internal Ratio
  float InternalRatio = [&] {
    return VerticalDist1 / (VerticalDist1 + VerticalDist2);
  } ();
  
  // Find Vector from v1 to Intersect Position
  Intersect = [&] {
    float m = InternalRatio;
    float n = 1.0f - InternalRatio;
    float x = (n * r1.x + m * r2.x) / (m + n);
    float y = (n * r1.y + m * r2.y) / (m + n);
    float z = (n * r1.z + m * r2.z) / (m + n);
    return Vec3f(x, y, z);
  } ();
  
  // It Checks whether the IntersectPos exist in the Polygon
  bool isExist = [&] {
    Vec3f cross1 = Vec3f(v3 - v1).cross(Vec3f(Intersect - v1)).normalized();
    Vec3f cross2 = Vec3f(v1 - v2).cross(Vec3f(Intersect - v2)).normalized();
    Vec3f cross3 = Vec3f(v2 - v3).cross(Vec3f(Intersect - v3)).normalized();
    
    double dot_12 = cross1.dot(cross2);
    double dot_13 = cross1.dot(cross3);
    
    if (dot_12 > 0 && dot_13 > 0) {
      return true;
    } else {
      return false;
    }
  } ();
  
  if (!isExist) return false;
  
  IntersectPos = Intersect;
  return true;
}

CINDER_APP_NATIVE(LineToFaceApp, RendererGl)
