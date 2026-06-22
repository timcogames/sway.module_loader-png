"use strict";

import { pathsToModuleNameMapper } from "ts-jest";
import type { JestConfigWithTsJest } from "ts-jest";
import * as tsConfig from "./tsconfig.json";

export default (async () => {
  const jestConfig: JestConfigWithTsJest = {
    roots: ["<rootDir>"],
    testEnvironment: "node",
    moduleDirectories: ['node_modules', '<rootDir>'],
    preset: "ts-jest",
    modulePaths: [tsConfig.compilerOptions.baseUrl],
    moduleFileExtensions: ["js", "ts"],
    moduleNameMapper: pathsToModuleNameMapper(tsConfig.compilerOptions.paths, { prefix: "<rootDir>/" }),
    transform: {
      "^.+\\.ts?$": "ts-jest"
    },
    testMatch: [
      `<rootDir>/tests/jest${process.env.USE_BINDING == "ON" ? "-embind" : ""}/*.spec.+(js|ts)`
    ]
  };

  return jestConfig;
})();
