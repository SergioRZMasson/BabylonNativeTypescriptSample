import { NativeEngine } from "@babylonjs/core/Engines/nativeEngine";
import { Scene } from "@babylonjs/core/scene";
import { ArcRotateCamera } from "@babylonjs/core/Cameras/arcRotateCamera";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { HemisphericLight } from "@babylonjs/core/Lights/hemisphericLight";
import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";

import "@babylonjs/core/Helpers/sceneHelpers";

// initialize babylon scene and engine
var engine = new NativeEngine();
var scene = new Scene(engine);

engine.runRenderLoop(function () {
  scene.render();
});

scene.createDefaultCamera(true, true, true);

var camera = scene.activeCamera as ArcRotateCamera;
camera.setTarget(Vector3.Zero());
camera.position = new Vector3(0, 5, -10);

// This creates a light, aiming 0,1,0 - to the sky (non-mesh)
var light = new HemisphericLight("light", new Vector3(0, 1, 0), scene);

// Default intensity is 1. Let's dim the light a small amount
light.intensity = 0.7;

// Our built-in 'sphere' shape.
var sphere = MeshBuilder.CreateSphere(
  "sphere",
  { diameter: 2, segments: 32 },
  scene
);

// Move the sphere upward 1/2 its height
sphere.position.y = 1;

// Our built-in 'ground' shape.
MeshBuilder.CreateGround("ground", { width: 6, height: 6 }, scene);
