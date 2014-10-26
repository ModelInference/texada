/*
 * settings.h
 *
 *  Created on: Oct 23, 2014
 *      Author: dennis
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

namespace texada {

/**
 * Represents a set of settings used to configure a linear checker.
 */
typedef struct settings {
    int sup_t;
    int sup_pot_t;
    float conf_t;
    bool use_global_t;
    bool print_full_stats;

    /**
     * Basic constructor which creates a set of default ("vanilla") settings
     */
    settings() {
        set_sup_t(0);
        set_sup_pot_t(0);
        set_conf_t(1.0);
        this->use_global_t = false;
        this->print_full_stats = false;
    }

    /**
     * Constructor which creates a settings struct from given configuration values
     */
    settings(int sup_t, int sup_pot_t, float conf_t, bool use_global_t, bool print_full_stats) {
        set_sup_t(sup_t);
        set_sup_pot_t(sup_pot_t);
        set_conf_t(conf_t);
        this->use_global_t = use_global_t;
        this->print_full_stats = print_full_stats;
    }

    /**
     * Setter for the support threshold.
     * Stops the program if an invalid (i.e. negative) support threshold has been passed in.
     * @param sup_t the support threshold
     */
    void set_sup_t(int sup_t) {
        if (sup_t < 0) {
            std::cerr << "support threshold cannot be negative." << "\n";
            exit(1);
        }
        this->sup_t = sup_t;
    }

    /**
     * Setter for the support potential threshold.
     * Stops the program if an invalid (i.e. negative) support potential threshold has been passed in.
     * @param sup_pot_t the support potential threhsold
     */
    void set_sup_pot_t(int sup_pot_t) {
        if (sup_pot_t < 0) {
            std::cerr << "support potential threshold cannot be negative." << "\n";
            exit(1);
        }
        this->sup_pot_t = sup_pot_t;
    }

    /**
     * Setter for the confidence threshold.
     * Stops the program if an invalid (i.e not between 0 and 1) confidence threshold has been passed in.
     * @param conf_t the confidence threshold
     */
    void set_conf_t(float conf_t) {
        if (conf_t < 0.0 || conf_t > 1.0) {
            std::cerr << "confidence threshold must be between 0 and 1." << "\n";
            exit(1);
        }
        this->conf_t = conf_t;
    }

    /**
     * Checks whether this settings struct describes a default (i.e. "vanilla") configuration.
     * @return
     */
    bool is_vanilla() {
        return sup_t == 0 &&
                sup_pot_t == 0 &&
                conf_t == 1.0 &&
                !print_full_stats;
    }

    /**
     * Checks whether a given set of statistics satisfies this settings' thresholds.
     * @return
     */
    bool satisfies_thresholds(statistic stat) {
        return stat.support >= sup_t &&
                stat.support_potential >= sup_pot_t &&
                stat.confidence() >= conf_t;
    }

} settings;

}

#endif /* SETTINGS_H_ */
