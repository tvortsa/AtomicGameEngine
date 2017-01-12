//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import HierarchyFrame = require("ui/frames/HierarchyFrame");
import InspectorUtils = require("ui/frames/inspector/InspectorUtils");
import ResourceOps = require("resources/ResourceOps");
import ModalOps = require("ui/modal/ModalOps");


class TerrainToolbar extends Atomic.UIWidget {

    constructor(parent: Atomic.UIWidget, properties: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/terraintoolbar.tb.txt");

        this.raiseButton = <Atomic.UIButton>this.getWidget("raisebutton");
        this.lowerButton = <Atomic.UIButton>this.getWidget("lowerbutton");
        this.smoothButton = <Atomic.UIButton>this.getWidget("smoothbutton");
     //   this.flattenButton = <Atomic.UIButton>this.getWidget("flattenbutton");

        this.brushPower = <Atomic.UIInlineSelect>this.getWidget("brushpower");
        this.brushHardness = <Atomic.UIInlineSelect>this.getWidget("brushhardness");
        this.brushSize = <Atomic.UIInlineSelect>this.getWidget("brushsize");

        this.subscribeToEvent(this, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));
        this.subscribeToEvent(EditorEvents.ActiveSceneEditorChange, (data) => this.handleActiveSceneEditorChanged(data));
        this.subscribeToEvent(EditorEvents.SceneClosed, (data) => this.handleSceneClosed(data));
        this.subscribeToEvent("TerrainEditModeChanged", (ev) => this.handleTerrainEditModeChanged(ev));
        
        this.raiseButton.setValue(1);

        parent.addChild(this);

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

     if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED && ev.target) {
            if (ev.target.id == "brushpower") {
             this.terrainEditor.setBrushPower(this.brushPower.value / 10);
             return true;
            }
            else if (ev.target.id == "brushhardness") {
             this.terrainEditor.setBrushHardness(this.brushHardness.value / 10);
             return true;
            }
            else if (ev.target.id == "brushsize") {
             this.terrainEditor.setBrushSize(this.brushSize.value);
            }
       }
       else if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK && ev.target) {

            if (ev.target.id == "raisebutton" || ev.target.id == "lowerbutton"
                 || ev.target.id == "flattenbutton" || ev.target.id == "smoothbutton") {
                var mode = 0;
                if (ev.target.id == "lowerbutton")
                    mode = 1;
                else if (ev.target.id == "smoothbutton")
                    mode = 2;
                // else if (ev.target.id == "flattenbutton"){
                //     mode = 3;
                // }

                this.sendEvent("TerrainEditModeChanged", { mode: mode });

                return true;

            }
       }
        return false;
    }

    handleSceneClosed(ev: EditorEvents.SceneClosedEvent) {
        if (ev.scene == this.scene) {
            this.remove();
        }
    }

    handleTerrainEditModeChanged(ev: TerrainEditModeChangedEvent) {

        this.raiseButton.value = 0;
        this.lowerButton.value = 0;
        this.smoothButton.value = 0;
     //   this.flattenButton.value = 0;

        switch (ev.mode) {
            case 0:
                this.raiseButton.value = 1;
                break;
            case 1:
                this.lowerButton.value = 1;
                break;
            case 2:
                this.smoothButton.value = 1;
                break;
            // case 3:
            //     this.flattenButton.value = 1;
            //     break;
        }

        this.terrainEditor.setTerrainEditMode(ev.mode);

    }


    handleActiveSceneEditorChanged(event: EditorEvents.ActiveSceneEditorChangeEvent) {

        if (!event.sceneEditor)
            return;

        this.sceneEditor = event.sceneEditor;
        this.scene = event.sceneEditor.scene;
        this.terrainEditor = this.sceneEditor.getTerrainEditor();

        this.terrainEditor.setBrushPower(0.5);
        this.terrainEditor.setBrushSize(5);
        this.terrainEditor.setBrushHardness(0.5);
        this.brushHardness.setValue(5);
        this.brushPower.setValue(5);
        this.brushSize.setValue(5);

        if (this.scene) {
            this.unsubscribeFromEvents(this.scene);
            return;
        }

        if (!event.sceneEditor)
            return;
    }

    modalOps: ModalOps;
    scene: Atomic.Scene = null;
    sceneEditor: Editor.SceneEditor3D;
    terrainEditor: Editor.TerrainEditor;
    raiseButton: Atomic.UIButton;
    lowerButton: Atomic.UIButton;
    smoothButton: Atomic.UIButton;
    flattenButton: Atomic.UIButton;
    brushHardness: Atomic.UIInlineSelect;
    brushPower: Atomic.UIInlineSelect;
    brushSize: Atomic.UIInlineSelect;
}

interface TerrainEditModeChangedEvent extends Atomic.NativeEvent {
        mode : number;
}

export = TerrainToolbar;


