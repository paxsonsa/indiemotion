// Copyright (c) 2021 Andrew Paxson. All rights reserved. Used under
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
/* kind.hpp 
*/
#pragma once
namespace indiemotion::messages 
{
        /**
         * @brief Represents the kind of messages available
         */
        static constexpr std::int32_t KindCount = 2;
        enum class Kind : std::int32_t
        {
            Acknowledgment = 0,
            // Error = 1,
            ListCameras
        };

        struct KindNames
        {
            inline static const std::string Acknowledgment = "Acknowledgment";
            inline static const std::string ListCameras = "ListCameras";
        };

        /**
         * @brief return a string name for the given kind
         * 
         * @param k the kind to transform into string
         * @return std::string 
         */
        std::string kindToStr(Kind k)
        {
            switch (k)
            {
            case Kind::Acknowledgment:
                return KindNames::Acknowledgment;
            // case kind::Error:
            //     return "Error";
            case Kind::ListCameras:
                return KindNames::ListCameras;
            }
        }
}