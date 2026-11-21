#pragma once

/** @file types.hpp  Vocabulary types: Coord, Index, WorldPt, CellPt, Rect, Pose2, OrientedRect, MapMeta, GridError, Connectivity. */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace gritty
{
    /**
     * @brief A point in the world frame.
     *
     * Plain aggregate, so `WorldPt{1.0, 2.0}` and designated initialisers work.
     * Values may be NaN or infinite; the conversions in Geometry check for this.
     */
    struct WorldPt
    {
        double x {};  ///< Position along the world x axis, in metres.
        double y {};  ///< Position along the world y axis, in metres.
    };

    /**
     * @brief A continuous position in the map frame, in cell units.
     *
     * Cell (u, v) covers [u, u+1) x [v, v+1) in this space, with its centre at (u+0.5, v+0.5).
     * Not an integer cell: see Coord. Not a world position: see WorldPt.
     * Values may be off-map, NaN or infinite.
     */
    struct CellPt
    {
        double u {};  ///< Position along map x (the +u axis), in cells.
        double v {};  ///< Position along map y (the +v axis), in cells.
    };

    /**
     * @brief An integer grid coordinate (a cell address), in the map frame.
     *
     * May lie off the map; Geometry::contains says whether it is inside.
     * Cell (0, 0) is the cell at the map origin. Not a world position: see WorldPt.
     */
    struct Coord
    {
        int32_t u;  ///< Cell column, along map x (+u), in cells.
        int32_t v;  ///< Cell row, along map y (+v), in cells.
    };

    using Index = std::size_t;

    struct Rect
    {
        int32_t u0 {};
        int32_t v0 {};
        int32_t width {};
        int32_t height{};
    };

    /**
     * @brief A rotated rectangle in the world frame.
     *
     * Used by the orientedRect traversals and orientedView.
     */
    struct OrientedRect
    {
        WorldPt center {};  ///< Rectangle centre in the world frame, in metres.
        double half_x {};   ///< Half the extent along the rectangle's own x axis, in metres.
        double half_y {};   ///< Half the extent along the rectangle's own y axis, in metres.
        double yaw {};      ///< Rotation of the rectangle about its centre, in radians.
    };

    /**
     * @brief An owning polygon in the world frame.
     *
     * Owns its vertices, so constructing or copying one allocates.
     */
    struct Polygon
    {
        std::vector<WorldPt> vertices;  ///< Vertices in order, in the world frame, in metres.
    };

    /**
     * @brief Which neighbours of a cell count as adjacent.
     */
    enum class Connectivity : std::uint8_t
    {
        Four,   ///< Edge neighbours only: E, N, W, S.
        Eight   ///< Edge and corner neighbours: E, N, W, S, then NE, NW, SW, SE.
    };

    /**
     * @brief Converts a neighbour count to a Connectivity.
     *
     * @param conn Neighbour count, 4 or 8.
     * @return Connectivity::Four for 4, Connectivity::Eight for 8, and Connectivity::Four
     *         for any other value.
     */
    inline Connectivity toConnectivity(std::uint8_t conn) {
        if (conn == 4) {
            return Connectivity::Four;
        }
        else if (conn == 8) {
            return Connectivity::Eight;
        }
        else {
            return Connectivity::Four; // Default to 4
        }
    }

    /**
     * @brief A position in the world frame, in metres.
     *
     * Holds the same data as WorldPt.
     */
    struct Pos2
    {
        // World Frame position
        double x {};
        double y {};
    };

    /**
     * @brief A 2D pose (position and heading) in the world frame.
     *
     * Used for the pose of the map frame in MapMeta::origin.
     */
    struct Pose2
    {
        // World Frame pose
        Pos2 pos {};   ///< Position in the world frame, in metres.
        double yaw {}; ///< Heading about the world z axis, in radians.
    };

    /**
     * @brief Shape and placement of a grid: size, cell size, pose and frame.
     *
     * Plain data
     */
    struct MetaData
    {
        // Default to 5cm (0.05m)
        double resolution {0.05};  ///< Cell size, in metres per cell.

        int32_t width {0};   ///< Number of cells along map x (+u).
        int32_t height {0};  ///< Number of cells along map y (+v).

        Pose2 origin {};  ///< Pose of the map frame in the world; its origin is the outer corner of cell (0, 0).
        std::string frame_id {"map"};  ///< Name of the world frame the origin is expressed in.

    };

    enum class GridError : std::uint8_t
    {
        
    };
}
