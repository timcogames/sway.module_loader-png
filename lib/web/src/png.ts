"use strict";

import { PNGPlugin } from "./plugin";

export type PrimTypeSet = boolean | number | string;

export interface IObjectWrap {
  /* eslint-disable */
  __parent: any;
  __construct(...args: Array<PrimTypeSet | {[prop: string]: PrimTypeSet}>): void;
  __destruct(): void;
}

export type IPNGPlugin = PNGPlugin.BaseIface;

export interface IPNGSubmodule {
  PNGPlugin: PNGPlugin.EmscriptenIface;
}
