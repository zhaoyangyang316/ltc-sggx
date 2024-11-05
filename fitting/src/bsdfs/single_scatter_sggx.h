#pragma once

#include <bsdf.h>
#include <sggx.h>

/*
    Analytic signle layer of sggx volume 

    - Only with Single-scattering.

*/

class SingleLayerSGGXBrdf : public Brdf {
public:
    SingleLayerSGGXBrdf(double alpha)
        : alpha(alpha) {}

    double eval_impl(const vec3 &wi, const vec3 &wo, Sampler *sampler) const override {
        const double density   = 1.0;
        const double thickness = 1.0;
        const double ss_albedo = 1.0;

        double cos_theta_i = wi.z(),
               cos_theta_o = wo.z();
        if (cos_theta_i <= 0.0 || cos_theta_o < 0.0) {
            return 0.0;
        }

        // Reparameterize alpha -> SGGX fiber cross section.
        double sigma = alpha * alpha;

        // Single scattering.
        sggx::Ellipsoid S = sggx::Ellipsoid::from_NonFiber(vec3(0.0, 0.0, 1.0), sigma);
        double lambda_wi = sggx::sigma(wi, S) / cos_theta_i,
               lambda_wo = sggx::sigma(wo, S) / cos_theta_o;

        double tmp0 = density*(lambda_wi + lambda_wo),
               tmp1 = 1.0 - std::exp(-thickness*tmp0);
        double tmp = tmp1 / (std::abs(cos_theta_i) * tmp0);

        double sigma_t_along_wi = density*sggx::sigma(wi, S),
               phase_function   = sggx::eval_phase_specular(wi, wo, S);

        double value = ss_albedo * phase_function * sigma_t_along_wi * tmp;

        if (!std::isfinite(value)) {
            /* Expressions `lambda_wi` and `lambda_wo` above tend
               towards infinity for grazing angles. */
            return 0.0;
        }

        // // Multiple-scattering.
        // auto [m, b] = mb(sigma);
        // double ms = std::exp(b - m * (std::sqrt(cos_theta_o * cos_theta_o + cos_theta_i * cos_theta_i)));
        // ms *= Ft(sigma, cos_theta_o) * Ft(sigma, cos_theta_i);
        // value += ms * cos_theta_o;

        return value;
    }

    std::pair<double, vec3> sample_impl(const vec3 &wi, Sampler *sampler) const override {
        throw std::runtime_error("\"SingleLayerSGGXBrdf::sample_impl\" not implemented.");
    }

    std::string to_string() const override {
        std::ostringstream out;
        out << "SingleLayerSGGXBrdf[" << std::endl;
        out << "  alpha = " << alpha << std::endl;
        out << "]";
        return out.str();
    }

public:
    double alpha;
};
