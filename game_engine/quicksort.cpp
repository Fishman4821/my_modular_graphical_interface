#pragma once

#include <iostream>

#include "common.cpp"

namespace quicksorts {
    void swap_Vec3(Vec3* a, Vec3* b) {
        Vec3 t = *a;
        *a = *b;
        *b = t;
    }

    int partition_Vec3(Vec3* arr, int low, int high, Vec3 point) {
        Vec3 pivot = arr[high];
        int i = low - 1;

        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j].distance_from(point) < pivot.distance_from(point))
            {
                i++;
                swap_Vec3(&arr[i], &arr[j]);
            }
        }

        swap_Vec3(&arr[i + 1], &arr[high]);
        return i + 1;
    }

    void _quickSort_Vec3(Vec3* arr, int low, int high, Vec3 point) {

        if (low < high) {
            int pi = partition_Vec3(arr, low, high, point);

            _quickSort_Vec3(arr, low, pi - 1, point);
            _quickSort_Vec3(arr, pi + 1, high, point);
        }
    }

    void swap_Vec3_Int(Vec3_Int* a, Vec3_Int* b) {
        Vec3_Int t = *a;
        *a = *b;
        *b = t;
    }

    bool order_v_map(Vec3_Int a, Vec3_Int b, Vec3* verts, int vert_count, Vec3 point, Vec3 camera_dir) {
        bool a_winding = __signbitf((verts[a.y] - verts[a.x]).cross(verts[a.z] - verts[a.y]).dot(camera_dir));
        bool b_winding = __signbitf((verts[b.y] - verts[b.x]).cross(verts[b.z] - verts[b.y]).dot(camera_dir));

        if (a_winding && !b_winding) {
            return true;
        } else if (!a_winding && b_winding) {
            return false;
        } else {
            return ((verts[a.x].distance_from(point) + verts[a.y].distance_from(point) + verts[a.z].distance_from(point)) / 3) < ((verts[b.x].distance_from(point) + verts[b.y].distance_from(point) + verts[b.z].distance_from(point)) / 3);
        }
    }

    int partition_v_map(Vec3_Int* v_map, int low, int high, Vec3* verts, int vert_count, Vec3 point, Vec3 camera_dir) {
        Vec3_Int pivot = v_map[high];
        int i = low - 1;

        int j;
        for (j = low; j <= high - 1; j++) {
            if (order_v_map(v_map[j], pivot, verts, vert_count, point, camera_dir)) {
                i++;
                swap_Vec3_Int(&v_map[i], &v_map[j]);
            }
        }

        swap_Vec3_Int(&v_map[i + 1], &v_map[high]);
        return i + 1;
    }

    void _quickSort_v_map(Vec3_Int* v_map, int low, int high, Vec3* verts, int vert_count, Vec3 point, Vec3 camera_dir) {

        if (low < high) {
            int pi = partition_v_map(v_map, low, high, verts, vert_count, point, camera_dir);

            _quickSort_v_map(v_map, low, pi - 1, verts, vert_count, point, camera_dir);
            _quickSort_v_map(v_map, pi + 1, high, verts, vert_count, point, camera_dir);
        }
    }

    void quickSort_v_map(Vec3_Int* v_map, int len, Vec3* verts, int vert_count, Vec3 point, Vec3 camera_dir) {
        _quickSort_v_map(v_map, 0, len - 1, verts, vert_count, point, camera_dir);
    }
}