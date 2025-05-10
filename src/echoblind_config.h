/**
 * @file echoblind_config.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef ECHOBLIND_CONFIG_H
#define ECHOBLIND_CONFIG_H

namespace echoblind::config {
    constexpr auto kProjectName = "${PROJECT_NAME}";
    constexpr auto kProjectDisplayName = "${PROJECT_DISPLAY_NAME}";
    constexpr auto kProjectVersion = "${PROJECT_VERSION}";
    constexpr auto kProjectVersionMajor = ${PROJECT_VERSION_MAJOR}u;
    constexpr auto kProjectVersionMinor = ${PROJECT_VERSION_MINOR}u;
    constexpr auto kProjectVersionPatch = ${PROJECT_VERSION_PATCH}u;
    constexpr auto kProjectBuildTimestamp = ${PROJECT_BUILD_TIMESTAMP}u;
    constexpr auto kProjectCommitSha = "${PROJECT_COMMIT_SHA}";
    constexpr auto kProjectDescription = "${PROJECT_DESCRIPTION}";
    constexpr auto kProjectHomepageUrl = "${PROJECT_HOMEPAGE_URL}";
    constexpr auto kProjectOrganizationName = "${PROJECT_ORGANIZATION_NAME}";
    constexpr auto kProjectOrganizationDomain = "${PROJECT_ORGANIZATION_DOMAIN}";
    constexpr auto kProjectAuthor = "${PROJECT_AUTHOR}";
}

#endif //ECHOBLIND_CONFIG_H
