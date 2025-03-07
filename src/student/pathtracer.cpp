
#include "../rays/pathtracer.h"
#include "../rays/samplers.h"
#include "../util/rand.h"
#include "debug.h"
#include <iostream>

namespace PT {

// Return the radiance along a ray entering the camera and landing on a
// point within pixel (x,y) of the output image.
//

// Pathtracer::trace_pixel must support super-sampling. The starter code provided to you
// will call Pathtracer::trace_pixel once for each sample (Pathtracer::n_samples) and
// resolve the results to compute final pixel values.
// Your implementation of Pathtracer::trace_pixel must choose a new location within the pixel for each sample.
// This is equivalent to saying that the ray tracer wil shoot n_samples camera rays per pixel.

Spectrum Pathtracer::trace_pixel(size_t x, size_t y) {

    Vec2 xy((float)x, (float)y);
    Vec2 wh((float)out_w, (float)out_h);

    // std::cout << x << " " << y << std::endl;
    // std::cout << out_w << " " << out_h << std::endl << std::endl;

    // TODO (PathTracer): Task 1

    // Generate a sample within the pixel with coordinates xy and return the
    // incoming light using trace_ray.

    // nomalize to (0, 1)
    // If n_samples is 1, please send the ray through the center of the pixel.
    // If n_samples > 1, please send the ray through any random point within the pixel
    Vec2 pixel_norm;
    float pdf;

    if (n_samples == 1) {
        pixel_norm = (xy.operator+(0.5)).operator/(wh); // center of pixel if 1
        //td::cout << pixel_norm << std::endl << std::endl;
    }
    else {
        // Tip: consider making a call to Samplers::Rect::Uniform
        Samplers::Rect::Uniform temp;
        // potentially temp.sample(pdf) can have same point for multiple samples and even get 1
        Vec2 sample_pt = temp.sample(pdf);
        pixel_norm = (xy.operator+(sample_pt)).operator/(wh);
        //std::cout << sample_pt << std::endl;
        //std::cout << pixel_norm << std::endl << std::endl;

    }
    // As an example, the code below generates a ray through the bottom left of the
    // specified pixel
    Ray out = camera.generate_ray(pixel_norm);

    // Tip: you may want to use log_ray for debugging. Given ray t, the following lines
    // of code will log .03% of all rays (see util/rand.h) for visualization in the app.
    // see student/debug.h for more detail.
    //if (RNG::coin_flip(0.03f))
    // log_ray(out, 10.0f);

    return trace_ray(out);
}

Spectrum Pathtracer::trace_ray(const Ray& ray) {

    
    // Lo += bsdf_s.emissive;
    //  (1) Ray objects have a depth field; if it reaches max_depth, you should
    //  terminate the path.
    // Trace ray into scene. If nothing is hit, sample the environment
    Trace hit = scene.hit(ray);
    if(!hit.hit) {
        if(env_light.has_value()) {
            return env_light.value().sample_direction(ray.dir);
        }
        return {};
    }
    log_ray(ray, hit.distance, ray.rcolor);
    if(ray.depth == max_depth) {
        return {};
    }
    // If we're using a two-sided material, treat back-faces the same as front-faces
    const BSDF& bsdf = materials[hit.material];
    if(!bsdf.is_sided() && dot(hit.normal, ray.dir) > 0.0f) {
        hit.normal = -hit.normal;
    }

    // Set up a coordinate frame at the hit point, where the surface normal becomes {0, 1, 0}
    // This gives us out_dir and later in_dir in object space, where computations involving the
    // normal become much easier. For example, cos(theta) = dot(N,dir) = dir.y!
    Mat4 object_to_world = Mat4::rotate_to(hit.normal);
    Mat4 world_to_object = object_to_world.T();
    Vec3 out_dir = world_to_object.rotate(ray.point - hit.position).unit();

    // Debugging: if the normal colors flag is set, return the normal color
    if(debug_data.normal_colors) return Spectrum::direction(hit.normal);

    // Now we can compute the rendering equation at this point.
    // We split it into two stages:
    //  1. sampling direct lighting (i.e. directly connecting the current path to
    //     each light in the scene)
    //  2. sampling the BSDF to create a new path segment

    // TODO (PathTracer): Task 4
    // The starter code sets radiance_out to (0.25,0.25,0.25) so that you can test your geometry
    // queries before you implement real lighting in Tasks 4 and 5. (i.e, anything that gets hit is not black.)
    // You should change this to (0,0,0) and accumulate the direct and indirect lighting computed below.
    Spectrum El = Spectrum(0.0f);
    {

        // lambda function to sample a light. Called in loop below.
        auto sample_light = [&](const auto& light) {
            // If the light is discrete (e.g. a point light), then we only need
            // one sample, as all samples will be equivalent
            int samples = light.is_discrete() ? 1 : (int)n_area_samples;
            for(int i = 0; i < samples; i++) {

                // Grab a sample of the light source. See rays/light.h for definition of this struct.
                // Most importantly for Task 4, it contains the distance to the light from hit.position.
                Light_Sample sample = light.sample(hit.position);
                Vec3 in_dir = world_to_object.rotate(sample.direction);

                // If the light is below the horizon, ignore it
                float cos_theta = in_dir.y;
                if(cos_theta <= 0.0f) continue;

                // If the BSDF has 0 throughput in this direction, ignore it.
                // This is another opportunity to do Russian roulette on low-throughput rays,
                // which would allow us to skip the shadow ray cast, increasing efficiency.
                Spectrum attenuation = bsdf.evaluate(out_dir, in_dir);
                if(attenuation.luma() == 0.0f) continue;

                // TODO (PathTracer): Task 4
                // Construct a shadow ray and compute whether the intersected surface is
                // in shadow. Only accumulate light if not in shadow.

                Ray sr(hit.position + sample.direction * EPS_F, sample.direction);
                auto strace = scene.hit(sr);
                float sRayToLight = (hit.position - strace.position).norm();
                if(strace.hit && sRayToLight < sample.distance) {

                    continue;
                }

                // Tip: since you're creating the shadow ray at the intersection point, it may
                // intersect the surface at time=0. Similarly, if the ray is allowed to have
                // arbitrary length, it will hit the light it was cast at. Therefore, you should
                // modify the time_bounds of your shadow ray to account for this. Using EPS_F is
                // recommended.

                // Note: that along with the typical cos_theta, pdf factors, we divide by samples.
                // This is because we're doing another monte-carlo estimate of the lighting from
                // area lights here.
                El += ray.beta * (cos_theta / (samples * sample.pdf)) * sample.radiance * attenuation;
            }
        };

        // If the BSDF is discrete (i.e. uses dirac deltas/if statements), then we are never
        // going to hit the exact right direction by sampling lights, so ignore them.
        if(!bsdf.is_discrete()) {

            // loop over all the lights and accumulate radiance.
            for(const auto& light : lights)
                sample_light(light);
            if(env_light.has_value())
                sample_light(env_light.value());

        }
    }
    
    Spectrum Lo = ray.Lo + El;
    // TODO (PathTracer): Task 5
    // Compute an indirect lighting estimate using path tracing with Monte Carlo.
    BSDF_Sample bsdf_s = bsdf.sample(out_dir);
    //Lo += ray.beta * bsdf_s.emissive;
    if(ray.depth == 0) {
        Lo += bsdf_s.emissive;
    } 
    

    float q = 0.25f;//1.0f - std::min(1.0f, Lo.luma());
    if (RNG::unit() < q){

        return Lo;
    }
    Ray ray_r(hit.position, object_to_world.rotate(bsdf_s.direction));
    ray_r.depth = ray.depth + 1;
    float dotN = dot(bsdf_s.direction, world_to_object.rotate(hit.normal));
    
    ray_r.beta = ray.beta * bsdf_s.attenuation * dotN/ bsdf_s.pdf;
    ray_r.beta *= 1.0f / (1 - q);
    ray_r.Lo = Lo;
    ray_r.rcolor = Spectrum(dotN > 0.0f ? 0.0f: 1.0f);
    
    //log_ray(ray_r, 1.0f, ray_r.rcolor);
    //log_ray(ray_r, 10.0f, Spectrum(1.0f, 0.0f, 0.0f));
    return trace_ray(ray_r);

    /* if(bsdf.is_mirror()) {
      //BSDF_Sample bsdf_s = bsdf.sample(out_dir);
      ro = radiance_out * bsdf_s.attenuation;

    }*/

    // (2) Randomly select a new ray direction (it may be reflection or transmittance
    // ray depending on surface type) using bsdf.sample()

    // (3) Compute the throughput of the recursive ray. This should be the current ray's
    // throughput scaled by the BSDF attenuation, cos(theta), and BSDF sample PDF.
    // Potentially terminate the path using Russian roulette as a function of the new throughput.
    // Note that allowing the termination probability to approach 1 may cause extra speckling.

    // (4) Create new scene-space ray and cast it to get incoming light. As with shadow rays, you
    // should modify time_bounds so that the ray does not intersect at time = 0. Remember to
    // set the new throughput and depth values.

    // (5) Add contribution due to incoming light with proper weighting. Remember to add in
    // the BSDF sample emissive term.
    //return radiance_out;
}

} // namespace PT
