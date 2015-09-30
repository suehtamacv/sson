#ifndef CONSTANT_H
#define CONSTANT_H

namespace Constant {
    /**
     * @brief alphaDCF is the loss in dB/km of the Dispersion Compensating Fiber.
     */
    static const long double alphaDCF = 0;

    /**
     * @brief alphaFB is the loss in dB/km of the Fiber.
     */
    static const long double alphaFB = 0.22;

    /**
     * @brief Dcr is the chromatic dispersion of the Fiber.
     */
    static const long double Dcr = -0.75;

    /**
     * @brief DDCF is the dispersion of the Dispersion Compensating Fiber.
     */
    static const long double DDCF = -99.77;

    /**
     * @brief c is the velocity of the light in vacuum.
     */
    static const long double c = 299792458;

    /**
     * @brief h is the Planck's constant.
     */
    static const long double h = 6.62606957E-34;
}

#endif /* CONSTANT_H */
