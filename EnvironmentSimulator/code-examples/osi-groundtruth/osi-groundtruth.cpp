#include "esminiLib.hpp"

#include "osi_common.pb.h"
#include "osi_object.pb.h"
#include "osi_groundtruth.pb.h"
#include "osi_version.pb.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    const osi3::GroundTruth* gt;

    SE_EnableOSIFile(0);  // 0 or "" will result in default filename, ground_truth.osi
    SE_SetOSIStaticReportMode(SE_OSIStaticReportMode::DEFAULT);

    SE_Init("../resources/xosc/cut-in_simple.xosc", 0, 1, 0, 0);

    // Fetch initial OSI struct, this will also update the OSI data
    gt = reinterpret_cast<const osi3::GroundTruth*>(SE_GetOSIGroundTruthRaw());

    // Lane boundaries (static road info only available in first OSI frame)
    printf("lane boundaries: %d\n", gt->lane_boundary_size());
    for (int j = 0; j < gt->lane_boundary_size(); j++)
    {
        printf("  lane boundary %d, nr of boundary points: %d\n", j, gt->lane_boundary(j).boundary_line_size());

#if 0  // change to 1 in order to print all boundary points
			for (int k = 0; k < gt->lane_boundary(j).boundary_line_size(); k++)
			{
				printf("    (%.2f, %.2f)\n",
					gt->lane_boundary(j).boundary_line(k).position().x(),
					gt->lane_boundary(j).boundary_line(k).position().y());
			}
#endif
    }

    for (int i = 0; i < 4; i++)
    {
        SE_StepDT(0.01f);  // ground truth will be automatically updated each step if we have either an OSIFile or fetched a pointer to the
                           // osi3::GroundTruth struct

        // Print timestamp
        printf("Frame %d timestamp: %.2f\n",
               i + 1,
               static_cast<double>(gt->timestamp().seconds()) + 1E-9 * static_cast<double>(gt->timestamp().nanos()));

        // Static content such as lane boundaries should be 0 at this point (we have SE_OSIStaticReportMode::DEFAULT, change to API or API_AND_LOG to
        // get static data)
        printf("lane boundaries: %d\n", gt->lane_boundary_size());

        // Road markings, e.g. zebra lines
        printf("road markings: %d\n", gt->road_marking_size());

        // Moving objects
        printf("moving objects: %d\n", gt->moving_object_size());

#if 1  // change to 1 in order to print some moving object state info
       // Print object id, position, orientation and velocity
        for (int j = 0; j < gt->moving_object().size(); j++)
        {
            printf("  obj id %u pos (%.2f, %.2f, %.2f) orientation (%.2f, %.2f, %.2f) vel (%.2f, %.2f, %.2f) acc (%.2f, %.2f, %.2f)\n",
                   static_cast<unsigned int>(gt->moving_object(j).id().value()),
                   gt->moving_object(j).base().position().x(),
                   gt->moving_object(j).base().position().y(),
                   gt->moving_object(j).base().position().z(),
                   gt->moving_object(j).base().orientation().yaw(),
                   gt->moving_object(j).base().orientation().pitch(),
                   gt->moving_object(j).base().orientation().roll(),
                   gt->moving_object(j).base().velocity().x(),
                   gt->moving_object(j).base().velocity().y(),
                   gt->moving_object(j).base().velocity().z(),
                   gt->moving_object(j).base().acceleration().x(),
                   gt->moving_object(j).base().acceleration().y(),
                   gt->moving_object(j).base().acceleration().z());
        }
#endif

        // Moving objects
        printf("stationary objects: %d\n", gt->stationary_object_size());
    }

    SE_Close();

    return 0;
}
