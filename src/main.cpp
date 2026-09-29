#include <iostream>
#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
float animT = 0.0f;
int selected = -1;

using Point2D = sf::Vector2f;

Point2D lerp(Point2D a, Point2D b, float t) { return a * (1 - t) + t * b; }
// TODO: (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<sf::Vector2f>& pts, float t) {
    Point2D a = lerp(pts[0], pts[1], t);
    Point2D b = lerp(pts[1], pts[2], t);
    Point2D c = lerp(pts[2], pts[3], t);
    Point2D d = lerp(a, b, t);
    Point2D e = lerp(b, c, t);
    Point2D f = lerp(d, e, t);
    return f;
}

// TODO: (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t) {
    Point2D a = lerp(pts[0], pts[1], t);
    Point2D b = lerp(pts[1], pts[2], t);
    Point2D c = lerp(pts[2], pts[3], t);
    Point2D d = lerp(a, b, t);
    Point2D e = lerp(b, c, t);
    return e - d;
}

// TODO: (Part 1) Store four control points for the curve.
std::vector<sf::Vector2f> points = {{100, 600}, {200, 100}, {600, 100}, {700, 600}};
// TODO: (Part 2) Track animation time for the square moving along the curve.
// TODO: (Part 3) Track the index of the control point being dragged.

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // TODO: (Part 3) On left-click, select the closest control point
            // using mouse->position and start dragging it.
            if (mouse->button == sf::Mouse::Button::Left) {
                int best = -1;
                float bestDist = 1e9f;
                for (int i = 0; i < points.size(); i++) {
                    float dist = (points[i] - sf::Vector2f(mouse->position)).length();
                    if (dist < bestDist) {
                        bestDist = dist;
                        best = i;
                    }
                }
                selected = best;
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // TODO: (Part 3) On left-button release, stop dragging.
            if (mouse->button == sf::Mouse::Button::Left) {
                selected = -1;
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            // TODO: (Part 3) Move the selected control point to mouse->position.
            if (selected != -1) {
                points[selected] = sf::Vector2f(mouse->position);
            }
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
            if (selected == 2 && points.size() > 4) {
                points[4] = points[3] + (points[3] - points[2]);
            }
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
            if (key->code == sf::Keyboard::Key::Equal) {
                points.push_back(points.back() + sf::Vector2f(0, -100));
                points.push_back(points.back() + sf::Vector2f(0, -100));
                points.push_back(points.back() + sf::Vector2f(0, -100));

            } else if (key->code == sf::Keyboard::Key::Hyphen && points.size() >= 7) {
                points.pop_back();
                points.pop_back();
                points.pop_back();
            }
        }
    }
}

// From Project DrawLine code
void DrawLine(sf::RenderWindow& window, Point2D from, Point2D to, float width, sf::Color c) {
    Point2D direction = (to - from).normalized();
    Point2D perpendicular(-direction.y, direction.x);
    Point2D offset = perpendicular * (width / 2.0f);

    Point2D p0 = from + offset;
    Point2D p1 = to + offset;
    Point2D p2 = to - offset;
    Point2D p3 = from - offset;

    sf::ConvexShape shape;
    shape.setPointCount(4);
    shape.setPoint(0, {p0.x, p0.y});
    shape.setPoint(1, {p1.x, p1.y});
    shape.setPoint(2, {p2.x, p2.y});
    shape.setPoint(3, {p3.x, p3.y});

    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    window.draw(shape);
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======
    /* Generalized in part 4.
    for (float t = 0; t < 1; t += 0.05f) {
        Point2D p1 = getPoint(points, t);
        Point2D p2 = getPoint(points, t + 0.05f);
        DrawLine(window, p1, p2, 2.0f, sf::Color::White);
    }
    */

    for (const auto& pt : points) {
        sf::CircleShape dot(5.0f);
        dot.setFillColor(sf::Color::Red);
        dot.setOrigin({5.0f, 5.0f});
        dot.setPosition(pt);
        window.draw(dot);
    }

    // ====== ====== ======
    // TODO: (Part 2) Draw a small square moving r  epeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    animT += 0.01f;
    if (animT >= 1.0f) {
        animT = 0.0f;
    }

    sf::RectangleShape square({20.0f, 20.0f});
    square.setFillColor(sf::Color::Green);
    square.setOrigin({10.0f, 10.0f});
    square.setRotation(getSlope(points, animT).angle());
    square.setPosition(getPoint(points, animT));
    window.draw(square);

    // ====== ====== ======
    // TODO: (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======
    /*
    DrawLine(window, points[0], points[1], 1.0f, sf::Color::Yellow);
    DrawLine(window, points[2], points[3], 1.0f, sf::Color::Yellow);
    */
    for (int k = 0; k < (points.size() - 1) / 3; k++) {
        std::vector<sf::Vector2f> seg = {points[3 * k], points[3 * k + 1], points[3 * k + 2],
                                         points[3 * k + 3]};
        for (float t = 0; t < 1; t += 0.05f) {
            Point2D p1 = getPoint(seg, t);
            Point2D p2 = getPoint(seg, t + 0.05f);
            DrawLine(window, p1, p2, 2.0f, sf::Color::White);
        }
        DrawLine(window, seg[0], seg[1], 1.0f, sf::Color::Yellow);
        DrawLine(window, seg[2], seg[3], 1.0f, sf::Color::Yellow);
    }

    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
