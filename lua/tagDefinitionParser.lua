-- SPDX-License-Identifier: GPL-3.0-only

local naming = require "naming"

---@class TagStructDefinition
---@field name string
---@field size number
---@field inherits string
---@field fields TagStructField[]
---@field type string

---@class TagStructField
---@field name string
---@field type string
---@field struct string?
---@field size number?
---@field count number
---@field cache_only boolean
---@field bounds boolean
---@field pointer boolean

---@class TagEnumDefinition
---@field name string
---@field options TagEnumOption[]|string[]
---@field type string

---@class TagEnumOption
---@field name string
---@field exclude boolean

---@class TagBitfieldDefinition
---@field name string
---@field width number
---@field fields TagBitfieldField[]|string[]
---@field type string

---@class TagBitfieldField
---@field name string

---@class ParsedTagStruct
---@field name string
---@field size integer
---@field inherits string
---@field fields ParsedTagStructField[]

---@class ParsedTagStructField
---@field name string
---@field type string
---@field struct string?
---@field size number
---@field cacheOnly boolean
---@field pointer boolean

---@class ParsedTagEnum
---@field name string
---@field values ParsedTagEnumField[]

---@class ParsedTagEnumField
---@field name string

---@class ParsedTagBitfield
---@field name string
---@field width string
---@field fields ParsedTagBitfieldField[]

---@class ParsedTagBitfieldField
---@field name string

---@class ParsedTagDefinition
---@field enums ParsedTagEnum[]
---@field bitfields ParsedTagBitfield[]
---@field structs ParsedTagStruct[]

local primitiveTypes = {
    "float",
    "int8_t",
    "int16_t",
    "int32_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "char",
    "void *"
}

local commonTypes = {
    float = { alias = "float", size = 4 },
    compressed_float = { alias = "int16_t", size = 2 },
    angle = { alias = "float", size = 4 },
    vector_2d = { alias = "VectorXY", size = 4*2 },
    compressed_vector_2d = { alias = "uint32_t", size = 4 },
    vector_2d_int = { alias = "VectorXYInt", size = 2*2 },
    vector_3d = { alias = "VectorXYZ", size = 4*3 },
    compressed_vector_3d = { alias = "uint32_t", size = 4 },
    euler_2d = { alias = "VectorPY", size = 4*2 },
    euler_3d = { alias = "VectorPYR", size = 4*3 },
    plane_3d = { alias = "Plane3D", size = 4*4 },
    plane_2d = { alias = "Plane2D", size = 4*3 },
    scenario_script_node_value = { alias = "ScenarioScriptNodeValue", size = 4 },
    quaternion = { alias = "Quaternion", size = 4*4 },
    address = { alias = "void *", size = 4 },
    tag_id = { alias = "TagHandle", size = 4 },
    string_32 = { alias = "String32", size = 32 },
    index = { alias = "uint16_t", size = 2 },
    color_rgb_float = { alias = "ColorRGB", size = 4*3 },
    color_argb_float = { alias = "ColorARGB", size = 4*4 },
    pixel_32 = { alias = "Pixel32", size = 4 },
    matrix_3x_3 = { alias = "Matrix", size = 4*3*3 },
    rectangle = { alias = "Rectangle2D", size = 2*4 },
    tag_reference = { alias = "TagReference", size = 4*4 },
    reflexive = { alias = "TagBlock", size = 4*5, template = true },
    file_data = { alias = "Data", size = 4*5 },
    utf16_string = { alias = "Data", size = 4*5 },
    bsp_vertex_data = { alias = "Data", size = 4*5 },
    int_8 = { alias = "int8_t", size = 1 },
    int_16 = { alias = "int16_t", size = 2 },
    int_32 = { alias = "int32_t", size = 4 },
    uint_8 = { alias = "uint8_t", size = 1 },
    uint_16 = { alias = "uint16_t", size = 2 },
    uint_32 = { alias = "uint32_t", size = 4 },
    tag_group = { alias = "TagGroup", size = 4 },
    id = { alias = "uint32_t", size = 4 }
}

local commonEnums = {
    framebuffer_blend_function = {size = 0x2},
    framebuffer_fade_mode = {size = 0x2},
    function_out = {size = 0x2},
    wave_function = {size = 0x2},
    material_type = {size = 0x2},
    function_type = {size = 0x2},
    function_bounds_mode = {size = 0x2},
    function_scale_by = {size = 0x2},
    function_name_nullable = {size = 0x2},
    grenade_type = {size = 0x2},
    vertex_type = {size = 0x2}
}

local commonBitfields = {
    is_unused_flag = {width = 0x4},
    is_unfiltered_flag = {width = 0x4},
    color_interpolation_flags = {width = 0x4}
}

local tagGroups = {
    "actor",
    "actor_variant",
    "antenna",
    "model_animations",
    "biped",
    "bitmap",
    "continuous_damage_effect",
    "model_collision_geometry",
    "color_table",
    "contrail",
    "device_control",
    "decal",
    "ui_widget_definition",
    "input_device_defaults",
    "device",
    "detail_object_collection",
    "effect",
    "equipment",
    "flag",
    "fog",
    "font",
    "material_effects",
    "garbage",
    "glow",
    "grenade_hud_interface",
    "hud_message_text",
    "hud_number",
    "hud_globals",
    "item",
    "item_collection",
    "damage_effect",
    "lens_flare",
    "lightning",
    "device_light_fixture",
    "light",
    "sound_looping",
    "device_machine",
    "globals",
    "meter",
    "light_volume",
    "gbxmodel",
    "model",
    "multiplayer_scenario_description",
    "preferences_network_game",
    "object",
    "particle",
    "particle_system",
    "physics",
    "placeholder",
    "point_physics",
    "projectile",
    "weather_particle_system",
    "scenario_structure_bsp",
    "scenery",
    "shader_transparent_chicago_extended",
    "shader_transparent_chicago",
    "scenario",
    "shader_environment",
    "shader_transparent_glass",
    "shader",
    "sky",
    "shader_transparent_meter",
    "sound",
    "sound_environment",
    "shader_model",
    "shader_transparent_generic",
    "ui_widget_collection",
    "shader_transparent_plasma",
    "sound_scenery",
    "string_list",
    "shader_transparent_water",
    "tag_collection",
    "camera_track",
    "dialogue",
    "unit_hud_interface",
    "unit",
    "unicode_string_list",
    "virtual_keyboard",
    "vector_font",
    "vector_font_data",
    "vehicle",
    "weapon",
    "wind",
    "weapon_hud_interface"
}

local isCommonType = function(typeName)
    for _, typeInfo in pairs(commonTypes) do
        if typeInfo.alias == typeName then
            return true
        end
    end
    return false
end

local isPrimitiveType = function(typeName)
    for _, primitiveType in ipairs(primitiveTypes) do
        if primitiveType == typeName then
            return true
        end
    end
    return false
end

-- Add exceptions to the naming convention
naming.addExceptions({"UTF16", "2D", "3D", "HUD", "ARGB", "RGB", "AI", "GBXModel", "UI", "BSP"})
naming.addExceptions({"R5G6B5", "A1R5G5B5", "A4R4G4B4", "X8R8G8B8", "A8R8G8B8", "DXT1", "DXT3", "DXT5", "A8Y8"})

---@param typeName string|nil
---@return string|nil
local function getNameForType(typeName)
    if not typeName then
        return typeName
    end
    local snakeCaseName = naming.toSnakeCase(typeName)
    if commonTypes[snakeCaseName] then
        return commonTypes[snakeCaseName].alias
    end
    return naming.toPascalCase(typeName)
end

---@param fieldName string|nil
---@return string|nil
local function getNameForField(fieldName) 
    if fieldName then
        local snakeCaseName = naming.toSnakeCase(fieldName)
        if snakeCaseName and snakeCaseName:find("%d") == 1 then
            snakeCaseName = "_" .. snakeCaseName
        end
        return snakeCaseName
    end
    return nil
end

---@param structDefinition TagStructDefinition
---@return ParsedTagStruct
local function parseStruct(structDefinition)
    local struct = {
        name = structDefinition.name,
        size = structDefinition.size,
        inherits = getNameForType(structDefinition.inherits),
        fields = {}
    }
    local fieldCount = 0
    for _, field in pairs(structDefinition.fields) do
        if(field.type ~= "editor_section") then
            local structField = {}
            structField.name = getNameForField(field.name)
            structField.type = getNameForType(field.type)
            structField.struct = getNameForType(field.struct)
            structField.size = field.size or field.count
            structField.cacheOnly = field.cache_only or false
            structField.pointer = field.pointer or false
            if field.bounds then
                structField.size = 2
            end
            fieldCount = fieldCount + 1
            struct.fields[fieldCount] = structField
        end
    end
    return struct
end

---@param enumDefinition TagEnumDefinition
---@return ParsedTagEnum
local function parseEnum(enumDefinition)
    local enum = {name = enumDefinition.name, values = {}}
    if enumDefinition.options then
        for index, option in pairs(enumDefinition.options) do
            local value
            if type(option) == "table" then
                value = option
                if value.name then
                    value.name = value.name
                end
            elseif type(option) == "string" then
                value = { name = option }
            end
            table.insert(enum.values, value)
        end
    end
    return enum
end

---@param bitfieldDefinition TagBitfieldDefinition
---@return ParsedTagBitfield
local function parseBitfield(bitfieldDefinition)
    local bitfield = {
        name = bitfieldDefinition.name, 
        width = bitfieldDefinition.width, 
        fields = {}
    }
    if bitfieldDefinition.fields then
        for _, field in pairs(bitfieldDefinition.fields) do
            local value
            if type(field) == "table" then
                value = field
                if value.name then
                    value.name = getNameForField(value.name) or error("Bitfield field name is nil")
                end
            elseif type(field) == "string" then
                value = { name = getNameForField(field) }
            end
            table.insert(bitfield.fields, value)
        end
    end
    return bitfield
end

---@param tagDefinition (TagStructDefinition|TagEnumDefinition|TagBitfieldDefinition)[]
---@return ParsedTagDefinition
local function parseTagDefinition(tagDefinition)
    local enums = {}
    local bitfields = {}
    local structs = {}
    for _, elementDefinition in pairs(tagDefinition) do
        if elementDefinition.type == "enum" then
            local enumDefinition = elementDefinition --[[@as TagEnumDefinition]]
            enums[#enums + 1] = parseEnum(enumDefinition)
        elseif elementDefinition.type == "bitfield" then
            local bitfieldDefinition = elementDefinition --[[@as TagBitfieldDefinition]]
            bitfields[#bitfields + 1] = parseBitfield(bitfieldDefinition)
        elseif elementDefinition.type == "struct" then
            local structDefinition = elementDefinition --[[@as TagStructDefinition]]
            structs[#structs + 1] = parseStruct(structDefinition)
        end
    end
    return {enums = enums, bitfields = bitfields, structs = structs}
end

local function getDependenciesForTagDefinition(definitions)
    local dependencies = {}

    local findType = function(typeName)
        if commonTypes[naming.toSnakeCase(typeName)] then
            return "common"
        elseif commonEnums[naming.toSnakeCase(typeName)] then
            return "common"
        elseif commonBitfields[naming.toSnakeCase(typeName)] then
            return "common"
        end

        for name, t in pairs(commonTypes) do
            if name == typeName or t.alias == typeName then
                return "common"
            end
        end
        
        for definitionName, definition in pairs(definitions) do
            for _, enum in ipairs(definition.enums) do
                if enum.name == typeName then
                    return definitionName
                end
            end
    
            for _, bitfield in ipairs(definition.bitfields) do
                if bitfield.name == typeName then
                    return definitionName
                end
            end
    
            for _, struct in ipairs(definition.structs) do
                if struct.name == typeName then
                    return definitionName
                end
            end
        end
        return nil
    end

    -- Check if everything is ok
    for definitionName, definition in pairs(definitions) do
        for _, struct in ipairs(definition.structs) do
            if struct.inherits then
                local dependency = findType(struct.inherits)
                if dependency then
                    if not dependencies[definitionName] then
                        dependencies[definitionName] = {}
                    end
                    dependencies[definitionName][dependency] = true
                end
            end
            for _, field in pairs(struct.fields) do
                if field.type == "reflexive" then
                    local dependency = findType(field.struct)
                    if dependency then
                        if not dependencies[definitionName] then
                            dependencies[definitionName] = {}
                        end
                        dependencies[definitionName][dependency] = true
                    end
                elseif field.type ~= "Pad" then
                    local dependency = findType(field.type)
                    if dependency then
                        if not dependencies[definitionName] then
                            dependencies[definitionName] = {}
                        end
                        dependencies[definitionName][dependency] = true
                    else
                        error("Struct " .. struct.name .. " has a field with an unknown type: " .. field.type)
                    end
                end
            end
        end
    end

    return dependencies
end

return {
    toSnakeCase = naming.toSnakeCase,
    toPascalCase = naming.toPascalCase,
    toCamelCase = naming.toCamelCase,
    toSentenceCase = naming.toSentenceCase,
    parseTagDefinition = parseTagDefinition,
    getDependenciesForTagDefinition = getDependenciesForTagDefinition,
    types = commonTypes,
    primitiveTypes = primitiveTypes,
    commonEnums = commonEnums,
    commonBitfields = commonBitfields,
    tagGroups = tagGroups,
    isCommonType = isCommonType,
    isPrimitiveType = isPrimitiveType
}
