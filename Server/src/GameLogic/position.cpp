#include "../../include/GameLogic/position.h"

Position::Position(
    double x,
    double y,
    double width,
    double height,
    double dim_x,
    double dim_y) :
    x(x),
    y(y),
    width(width),
    height(height),
    dim_x(dim_x),
    dim_y(dim_y) {
}

bool Position::collides(const Position &other) const {
    std::tuple<double, double, bool> area_other_x = other.getXArea();
    std::tuple<double, double, bool> area_other_y = other.getYArea();
    std::tuple<double, double, bool> area_x = getXArea();
    std::tuple<double, double, bool> area_y = getYArea();

    bool hits_x = false;
    bool hits_y = false;

    // ambos son complementos quiere decir que ambos ocupan los limites del mapa
    if (!std::get<2>(area_other_x) && !std::get<2>(area_x)) {
        hits_x = true;

    // solo area_x es complemento, Tengo que ver si other está fuera.
    } else if (std::get<2>(area_other_x) && !std::get<2>(area_x)) {
        double x_min_other = std::get<0>(area_other_x);
        double x_max_other = std::get<1>(area_other_x);
        double x_min = std::get<0>(area_x);
        double x_max = std::get<1>(area_x);
        hits_x = !((x_min <= x_max_other) && (x_max_other <= x_max)) || (((x_min <= x_min_other) && (x_min_other <= x_max)));

    // solo area_other_x es complemento, Tengo que ver si area está fuera
    } else if (!std::get<2>(area_other_x) && std::get<2>(area_x)) {
        double x_min_other = std::get<0>(area_other_x);
        double x_max_other = std::get<1>(area_other_x);
        double x_min = std::get<0>(area_x);
        double x_max = std::get<1>(area_x);
        hits_x = !((x_min_other <= x_max) && (x_max <= x_max_other)) || (((x_min_other <= x_min) && (x_min <= x_max_other)));

    // ninguno es complemento, Tengo que comparar normal
    } else if (std::get<2>(area_other_x) && std::get<2>(area_x)) {
        double x_min_other = std::get<0>(area_other_x);
        double x_max_other = std::get<1>(area_other_x);
        double x_min = std::get<0>(area_x);
        double x_max = std::get<1>(area_x);
        hits_x = ((x_min <= x_max_other) && (x_max_other <= x_max)) || (((x_min <= x_min_other) && (x_min_other <= x_max)));
    }

    // ambos son complementos quiere decir que ambos ocupan los limites del mapa
    if (!std::get<2>(area_other_y) && !std::get<2>(area_y)) {
        hits_y = true;

    // solo area_y es complemento, Tengo que ver si other está fuera.
    } else if (std::get<2>(area_other_y) && !std::get<2>(area_y)) {
        double y_min_other = std::get<0>(area_other_y);
        double y_max_other = std::get<1>(area_other_y);
        double y_min = std::get<0>(area_y);
        double y_max = std::get<1>(area_y);
        hits_y = !((y_min <= y_max_other) && (y_max_other <= y_max)) || (((y_min <= y_min_other) && (y_min_other <= y_max)));

    // solo area_other_y es complemento, Tengo que ver si area está fuera
    } else if (!std::get<2>(area_other_y) && std::get<2>(area_y)) {
        double y_min_other = std::get<0>(area_other_y);
        double y_max_other = std::get<1>(area_other_y);
        double y_min = std::get<0>(area_y);
        double y_max = std::get<1>(area_y);
        hits_y = !((y_min_other <= y_max) && (y_max <= y_max_other)) || (((y_min_other <= y_min) && (y_min <= y_max_other)));

    // ninguno es complemento, Tengo que comparar normal
    } else if (std::get<2>(area_other_y) && std::get<2>(area_y)) {
        double y_min_other = std::get<0>(area_other_y);
        double y_max_other = std::get<1>(area_other_y);
        double y_min = std::get<0>(area_y);
        double y_max = std::get<1>(area_y);
        hits_y = ((y_min <= y_max_other) && (y_max_other <= y_max)) || (((y_min <= y_min_other) && (y_min_other <= y_max)));
    }
    return (hits_x && hits_y);
}


double Position::getXPos(void) const {
    return x;
}
double Position::getYPos(void) const {
    return y;
}

double Position::getWidth(void) const {
    return width;
}

double Position::getHeight(void) const {
    return height;
}

std::tuple<double, double, bool> Position::getXArea() const{
    // como el mapa es circular, indico los limites de la pos
    // y con el booleano si es el area real o el complemento
    double x_max = x + width * 0.5;
    if (x_max > dim_x) x_max = x_max - dim_x;
    double x_min = x - width * 0.5;
    if (x_min < 0) x_min = x_min + dim_x;

    if (x_max < x_min) return std::tuple<double, double, bool>{x_max, x_min, false};
    return std::tuple<double, double, bool>{x_min, x_max, true};
}

std::tuple<double, double, bool> Position::getYArea() const{
    // como el mapa es circular, indico los limites de la pos
    // y con el booleano si es el area real o el complemento
    double y_max = y + height * 0.5;
    if (y_max > dim_y) y_max = y_max - dim_y;
    double y_min = y - height * 0.5;
    if (y_min < 0) y_min = y_min + dim_y;

    if (y_max < y_min) return std::tuple<double, double, bool>{y_max, y_min, false};
    return std::tuple<double, double, bool>{y_min, y_max, true};
} 

void Position::setXPos(double new_x) {
    x = new_x;
}

void Position::setYPos(double new_y) {
    y = new_y;
}

bool Position::operator==(const Position &other) const {
    return ((this->getXPos() == other.getXPos()) && (this->getYPos() == other.getYPos()));
}
