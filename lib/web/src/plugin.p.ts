"use strict";


export interface BaseIface {
  loadFromArrayBuffer(buf: ArrayBuffer): any;
}

export interface EmscriptenIface extends BaseIface {
  new(): EmscriptenClass;
}

export declare class EmscriptenClass implements BaseIface {
  constructor();

  loadFromArrayBuffer(buf: ArrayBuffer): any;
}
