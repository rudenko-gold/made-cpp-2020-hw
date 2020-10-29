#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

const double PI = 3.14159265;
const double EPS = 1e-6;

double calc_3x3_det(std::vector<double> matrix) {
  return matrix[0] * matrix[4] * matrix[8] + matrix[1] * matrix[5] * matrix[6] +
         matrix[2] * matrix[3] * matrix[7] - matrix[2] * matrix[4] * matrix[6] -
         matrix[0] * matrix[5] * matrix[7] - matrix[1] * matrix[3] * matrix[8];
}

double grad_to_rad(double grad) { return (grad * PI) / 180; }

struct Point {
  Point() = default;
  Point(double x, double y) : x(x), y(y) {}

  double x = 0.0;
  double y = 0.0;
};

bool operator==(const Point& lhs, const Point& rhs) {
  return fabs(lhs.x - rhs.x) < EPS && fabs(lhs.y - rhs.y) < EPS;
}

bool operator!=(const Point& lhs, const Point& rhs) { return !(lhs == rhs); }

double distance_between(const Point& lhs, const Point& rhs) {
  return sqrt((lhs.x - rhs.x) * (lhs.x - rhs.x) +
              (lhs.y - rhs.y) * (lhs.y - rhs.y));
}

Point rotate(Point point, Point center, double angle) {
  angle = grad_to_rad(angle);
  Point result;

  result.x = center.x + (point.x - center.x) * std::cos(angle) -
             (point.y - center.y) * std::sin(angle);
  result.y = center.y + (point.y - center.y) * std::cos(angle) +
             (point.x - center.x) * std::sin(angle);

  return result;
}

Point reflex(Point point, Point center) {
  return {2.0 * center.x - point.x, 2.0 * center.y - point.y};
}

Point scale(Point point, Point center, double coef) {
  Point scaled_vec = {coef * (point.x - center.x), coef * (point.y - center.y)};
  return {center.x + scaled_vec.x, center.y + scaled_vec.y};
}

class Line {
 public:
  Line() : angle(0.0), offset(0.0) {}

  Line(Point lhs, Point rhs) {
    this->angle = (rhs.y - lhs.y) / (rhs.x - lhs.x);
    this->offset = (rhs.x * lhs.y - lhs.x * rhs.y) / (rhs.x - lhs.x);
  }

  Line(double angle_coef, double offset_coef)
      : angle(angle_coef), offset(offset_coef) {}

  Line(Point point, double angle_coef) : angle(angle_coef) {
    this->offset = point.y - angle_coef * point.x;
  }

  bool operator==(const Line& other) const {
    return fabs(this->angle - other.angle) < EPS &&
           fabs(this->offset - other.offset) < EPS;
  }

  bool operator!=(const Line& other) const { return !(*this == other); }

  Line get_normal(const Point& point) const {
    return Line(point, -1.0 / this->angle);
  }

  Point intersect(const Line& other) const {
    double x = (other.offset - this->offset) / (this->angle - other.angle);
    double y = this->angle * x + this->offset;

    return {x, y};
  }

 private:
  double angle;
  double offset;
};

Point reflex(Point point, Line line) {
  Line normal = line.get_normal(point);
  Point center = line.intersect(normal);

  return reflex(point, center);
}

class Polygon;
class Ellipse;

class Shape {
 public:
  virtual double perimeter() const = 0;
  virtual double area() const = 0;

  virtual bool operator==(const Shape& another) const = 0;
  virtual bool operator!=(const Shape& another) const = 0;

  virtual void rotate(Point center, double angle) = 0;
  virtual void reflex(Point center) = 0;
  virtual void reflex(Line axis) = 0;
  virtual void scale(Point center, double coefficient) = 0;

  virtual bool isEqualToPolygon(const Polygon& other) const = 0;
  virtual bool isEqualToEllipse(const Ellipse& other) const = 0;
};

class Polygon : public Shape {
 public:
  Polygon(std::vector<Point> points) : vertices(std::move(points)) {
    double s_area = 0.0;

    for (size_t i = 0; i < vertices.size(); ++i) {
      s_area += (vertices[i].x - vertices[(i + 1) % vertices.size()].x) *
                (vertices[i].y + vertices[(i + 1) % vertices.size()].y);
    }

    if (s_area < 0) {
      std::reverse(vertices.begin(), vertices.end());
    }

    size_t pos = findLowerLeftPoint(vertices);
    vertices = leftRotate(vertices, pos);
    // now each vertices array starts from
    // the leftest and the most bottom point
  }

  size_t verticesCount() const { return vertices.size(); }

  const std::vector<Point>& getVertices() const { return vertices; }

  double perimeter() const override {
    double result = 0.0;

    for (size_t i = 0; i < vertices.size(); ++i) {
      result +=
          distance_between(vertices[i], vertices[(i + 1) % vertices.size()]);
    }

    return result;
  }

  double area() const override { return fabs(signedArea()); }

  bool operator==(const Shape& another) const override {
    return another.isEqualToPolygon(*this);
  }

  bool operator!=(const Shape& another) const override {
    return !(another.isEqualToPolygon(*this));
  }

  bool isEqualToPolygon(const Polygon& other) const override {
    return this->vertices == other.vertices;
  }

  bool isEqualToEllipse(const Ellipse& other) const override { return false; }

  void rotate(Point center, double angle) override {
    for (auto& point : vertices) {
      point = ::rotate(point, center, angle);
    }
  }

  void reflex(Point center) override {
    auto new_vertices = vertices;

    for (auto& point : new_vertices) {
      point = ::reflex(point, center);
    }

    *this = Polygon(new_vertices);
  }

  void reflex(Line axis) override {
    auto new_vertices = vertices;

    for (auto& point : new_vertices) {
      point = ::reflex(point, axis);
    }

    *this = Polygon(new_vertices);
  }

  void scale(Point center, double coefficient) override {
    auto new_vertices = vertices;

    for (auto& point : new_vertices) {
      point = ::scale(point, center, coefficient);
    }

    *this = Polygon(new_vertices);
  }

 protected:
  size_t findLowerLeftPoint(const std::vector<Point>& points) const {
    size_t result_idx = 0;

    for (size_t i = 1; i < points.size(); ++i) {
      if (points[i].y < points[result_idx].y) {
        result_idx = i;
      } else if (points[i].y == points[result_idx].y) {
        if (points[i].x < points[result_idx].x) {
          result_idx = i;
        }
      }
    }

    return result_idx;
  }

  std::vector<Point> leftRotate(const std::vector<Point>& points,
                                size_t start_idx) const {
    std::vector<Point> result(points.begin() + start_idx, points.end());
    result.insert(result.end(), points.begin(), points.begin() + start_idx);
    return result;
  }

  std::vector<Point> vertices;

 private:
  double signedArea() const {
    double result = 0.0;

    for (size_t i = 0; i < vertices.size(); ++i) {
      result += (vertices[i].x - vertices[(i + 1) % vertices.size()].x) *
                (vertices[i].y + vertices[(i + 1) % vertices.size()].y);
    }

    return result;
  }
};

class Ellipse : public Shape {
 public:
  Ellipse(Point focus_lhs, Point focus_rhs, double distance)
      : focus_lhs(focus_lhs), focus_rhs(focus_rhs), distance(distance) {}

  std::pair<Point, Point> focuses() const { return {focus_lhs, focus_rhs}; }

  double eccentricity() const { return focal_distance() / big_half_axis(); }

  Point center() const {
    return {(focus_lhs.x + focus_rhs.x) / 2.0,
            (focus_lhs.y + focus_rhs.y) / 2.0};
  }

  double perimeter() const override {
    return 4.0 * big_half_axis() * std::comp_ellint_2(eccentricity());
  }

  double area() const override {
    return PI * big_half_axis() * small_half_axis();
  }

  bool operator==(const Shape& another) const override {
    return another.isEqualToEllipse(*this);
  }

  bool operator!=(const Shape& another) const override {
    return !(another.isEqualToEllipse(*this));
  }

  bool isEqualToPolygon(const Polygon& other) const override { return false; }

  bool isEqualToEllipse(const Ellipse& other) const override {
    return this->center() == other.center() &&
           this->small_half_axis() == other.small_half_axis() &&
           this->big_half_axis() == other.big_half_axis();
  }

  void rotate(Point center, double angle) override {
    this->focus_lhs = ::rotate(this->focus_lhs, center, angle);
    this->focus_rhs = ::rotate(this->focus_rhs, center, angle);
  }

  void reflex(Point center) override {
    this->focus_lhs = ::reflex(this->focus_lhs, center);
    this->focus_rhs = ::reflex(this->focus_rhs, center);
  }

  void reflex(Line axis) override {
    this->focus_lhs = ::reflex(this->focus_lhs, axis);
    this->focus_rhs = ::reflex(this->focus_rhs, axis);
  }

  void scale(Point center, double coefficient) override {
    focus_lhs = ::scale(focus_lhs, center, coefficient);
    focus_rhs = ::scale(focus_rhs, center, coefficient);

    distance *= coefficient;
  }

 protected:
  double focal_distance() const {
    return distance_between(focus_lhs, focus_rhs) / 2.0;
  }

  double big_half_axis() const { return distance / 2.0; }

  double small_half_axis() const {
    return big_half_axis() * std::sqrt(1.0 - eccentricity() * eccentricity());
  }

  Point focus_lhs;
  Point focus_rhs;
  double distance;
};

class Circle : public Ellipse {
 public:
  Circle(Point center, double radius) : Ellipse(center, center, radius * 2.0) {}

  double radius() const { return distance / 2.0; }

  std::vector<Point> intersect(const Circle& other) const {
    if (other.center() == this->center()) {
      if (other.radius() == this->radius()) {
        throw std::invalid_argument("Circles are same");
      } else {
        return {};
      }
    }

    auto candidates = intersect(-2.0 * (other.center().x - this->center().x),
                                -2.0 * (other.center().y - this->center().y),
                                (other.center().x - this->center().x) *
                                        (other.center().x - this->center().x) +
                                    (other.center().y - this->center().y) *
                                        (other.center().y - this->center().y) +
                                    this->radius() * this->radius() -
                                    other.radius() * other.radius());

    for (auto& point : candidates) {
      point.x += center().x;
      point.y += center().y;
    }

    return candidates;
  }

  std::vector<Point> intersect(double a, double b, double c) const {
    double r = this->radius();

    double x0 = -a * c / (a * a + b * b);
    double y0 = -b * c / (a * a + b * b);

    if (c * c > (r * r * (a * a + b * b) + EPS)) {
      return {};
    } else if (fabs(c * c - r * r * (a * a + b * b)) < EPS) {
      return {{x0, y0}};
    } else {
      double d = r * r - c * c / (a * a + b * b);
      double mult = std::sqrt(d / (a * a + b * b));

      double ax = x0 + b * mult;
      double bx = x0 - b * mult;
      double ay = y0 - a * mult;
      double by = y0 + a * mult;

      return {{ax, ay}, {bx, by}};
    }
  }
};

bool counter_clockwise(Point p1, Point p2, Point p3) {
  return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) > 0.0;
}

std::vector<Point> buildRectangle(Point lhs, Point rhs,
                                  double coef_between_sides) {
  if (coef_between_sides < EPS) {
    throw std::invalid_argument("Coefficient between sides is invalid (= 0)");
  }

  double diag = distance_between(lhs, rhs);

  double b =
      std::sqrt((diag * diag) / (coef_between_sides * coef_between_sides + 1));
  double a = coef_between_sides * b;

  if (a > b) {
    std::swap(a, b);
  }

  Circle small_circle(lhs, a);
  Circle big_circle(rhs, b);

  auto candidates = small_circle.intersect(big_circle);

  if (candidates.size() == 2) {
    Point third_point;

    if (counter_clockwise(lhs, rhs, candidates[0])) {
      third_point = candidates[0];
    } else {
      third_point = candidates[1];
    }

    Point middle_of_diag = {(lhs.x + rhs.x) / 2.0, (lhs.y + rhs.y) / 2.0};

    Point fourth_point = ::rotate(third_point, middle_of_diag, 180.0);

    return {lhs, third_point, rhs, fourth_point};
  } else {
    throw std::invalid_argument("Points are invalid" +
                                std::to_string(candidates.size()));
  }
}

class Rectangle : public Polygon {
 public:
  Rectangle(Point lhs, Point rhs, double coef_between_sides)
      : Polygon(buildRectangle(lhs, rhs, coef_between_sides)) {
    center_ = {(lhs.x + rhs.x) / 2.0, (lhs.y + rhs.y) / 2.0};
    diags = {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
  }

  Point center() const { return center_; }

  std::pair<Line, Line> diagonals() const { return diags; }

 protected:
  Point center_;
  std::pair<Line, Line> diags;
};

class Square : public Rectangle {
 public:
  Square(Point lhs, Point rhs) : Rectangle(lhs, rhs, 1.0) {}

  Circle circumscribedCircle() const {
    return Circle(center(), perimeter() / (4.0 * std::sqrt(2.0)));
  }

  Circle inscribedCircle() const { return Circle(center(), perimeter() / 8.0); }
};

class Triangle : public Polygon {
 public:
  Triangle(Point point_1, Point point_2, Point point_3)
      : Polygon({point_1, point_2, point_3}) {}

  Circle circumscribedCircle() const {
    double a = distance_between(vertices[1], vertices[2]);
    double b = distance_between(vertices[0], vertices[2]);
    double c = distance_between(vertices[0], vertices[1]);

    double p = (a + b + c) / 2.0;

    double radius =
        (a * b * c) / (4 * std::sqrt(p * (p - a) * (p - b) * (p - c)));

    double x0_up_det = calc_3x3_det(
        {((vertices[0].x * vertices[0].x) + (vertices[0].y + vertices[0].y)),
         vertices[0].y, 1.0,
         ((vertices[1].x * vertices[1].x) + (vertices[1].y + vertices[1].y)),
         vertices[1].y, 1.0,
         ((vertices[2].x * vertices[2].x) + (vertices[2].y + vertices[2].y)),
         vertices[2].y, 1.0});

    double y0_up_det = calc_3x3_det(
        {vertices[0].x,
         (vertices[0].x * vertices[0].x + vertices[0].y * vertices[0].y), 1.0,
         vertices[1].x,
         (vertices[1].x * vertices[1].x + vertices[1].y * vertices[1].y), 1.0,
         vertices[2].x,
         (vertices[2].x * vertices[2].x + vertices[2].y * vertices[2].y), 1.0});

    double bottom_det =
        calc_3x3_det({vertices[0].x, vertices[0].y, 1.0, vertices[1].x,
                      vertices[1].y, 1.0, vertices[2].x, vertices[2].y, 1.0});

    Point center = {x0_up_det / (2.0 * bottom_det),
                    y0_up_det / (2.0 * bottom_det)};

    return Circle(center, radius);
  }

  Circle inscribedCircle() const {
    double a = distance_between(vertices[1], vertices[2]);
    double b = distance_between(vertices[0], vertices[2]);
    double c = distance_between(vertices[0], vertices[1]);

    Point center = {
        (a * vertices[0].x + b * vertices[1].x + c * vertices[2].x) /
            (a + b + c),
        (a * vertices[0].y + b * vertices[1].y + c * vertices[2].y) /
            (a + b + c)};

    double p = (a + b + c) / 2.0;
    double radius = std::sqrt(((p - a) * (p - b) * (p - c)) / p);

    return Circle(center, radius);
  }

  Point centroid() const {
    Point result;

    for (const auto& point : vertices) {
      result.x += point.x;
      result.y += point.y;
    }

    result.x /= (double)vertices.size();
    result.y /= (double)vertices.size();

    return result;
  }

  Point orthocenter() const {
    double x0_up_det = calc_3x3_det(
        {vertices[0].y,
         vertices[1].x * vertices[2].x + vertices[0].y * vertices[0].y, 1.0,
         vertices[1].y,
         vertices[0].x * vertices[2].x + vertices[1].y * vertices[1].y, 1.0,
         vertices[2].y,
         vertices[0].x * vertices[1].x + vertices[2].y * vertices[2].y, 1.0});

    double y0_up_det = calc_3x3_det(
        {vertices[0].x * vertices[0].x + vertices[1].y * vertices[2].y,
         vertices[0].x, 1.0,
         vertices[1].x * vertices[1].x + vertices[0].y * vertices[2].y,
         vertices[1].x, 1.0,
         vertices[2].x * vertices[2].x + vertices[0].y * vertices[1].y,
         vertices[2].x, 1.0});

    double bottom_det =
        calc_3x3_det({vertices[0].x, vertices[0].y, 1.0, vertices[1].x,
                      vertices[1].y, 1.0, vertices[2].x, vertices[2].y, 1.0});

    Point center = {x0_up_det / bottom_det, y0_up_det / bottom_det};

    return center;
  }

  Line EulerLine() const { return Line(centroid(), orthocenter()); }

  Circle ninePointsCircle() const {
    Circle result = circumscribedCircle();
    result.scale(orthocenter(), 0.5);
    return result;
  }
};
