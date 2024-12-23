-- SPDX-License-Identifier: GPL-3.0-only

---@class DefinitionElement
---@field name string
---@field class string
---@field options string[]
---@field type string | '"struct"' | '"enum"' | '"pad"' | '"bitfield"' | '"Angle"' | '"Fraction"' | '"TagEnum"' | '"TagString"' | '"TagIndex"' | '"unint8"' | '"unint16"' | '"unint32"' | '"int8"' | '"int16"' | '"int32"' | '"float"'
---@field size number
---@field width number
---@field count number
---@field bounds number
---@field fields DefinitionElement[]
---@field struct string
---@field description string
---@field inherits string
---@field cache_only boolean

---@class StructField
---@field name string
---@field type string
---@field pointer boolean
---@field size number
---@field description string
---@field struct string

---@class Struct
---@field name string
---@field width number
---@field inherits string
---@field fields StructField[]

---@class BitfieldStruct
---@field name string
---@field type string
---@field fields string[]

---@class Enum
---@field name string
---@field values string[]

---@class TagDefinition
---@field structs Struct[]
---@field enums Enum[]
---@field bitfields BitfieldStruct[]

---@class DataType
---@field size number
---@field alias string
---@field template? boolean

---@type table<string, DataType>
local commonTypes = {
    float = { alias = "float", size = 4 },
    Angle = { alias = "float", size = 4 },
    fraction = { alias = "float", size = 4 },
    vector2_d = { alias = "VectorIJ", size = 4*2 },
    vector3_d = { alias = "VectorIJK", size = 4*3 },
    euler2_d = { alias = "VectorPY", size = 4*2 },
    euler3_d = { alias = "VectorPYR", size = 4*3 },
    point3_d = { alias = "VectorXYZ", size = 4*3 },
    point2_d = { alias = "VectorXY", size = 4*2 },
    plane3_d = { alias = "Plane3D", size = 4*4 },
    plane2_d = { alias = "Plane2D", size = 4*3 },
    scenario_script_node_value = { alias = "ScenarioScriptNodeValue", size = 4 },
    quaternion = { alias = "Quaternion", size = 4*4 },
    point2_d_int = { alias = "VectorXYInt", size = 2*2 },
    pointer = { alias = "void *", size = 4 },
    tag_i_d = { alias = "TagHandle", size = 4 },
    string32 = { alias = "String32", size = 32 },
    index = { alias = "uint16_t", size = 2 },
    color_r_g_b = { alias = "ColorRGB", size = 4*3 },
    color_a_r_g_b = { alias = "ColorARGB", size = 4*4 },
    color_a_r_g_b_int = { alias = "ColorARGBInt", size = 4 },
    matrix = { alias = "Matrix", size = 4*3*3 },
    rectangle = { alias = "Rectangle2D", size = 2*4 },
    tag_reference = { alias = "TagReference", size = 4*4 },
    reflexive = { alias = "TagBlock", size = 4*5, template = true },
    data = { alias = "Data", size = 4*5 },
    int8 = { alias = "int8_t", size = 1 },
    int16 = { alias = "int16_t", size = 2 },
    int32 = { alias = "int32_t", size = 4 },
    uint8 = { alias = "uint8_t", size = 1 },
    uint16 = { alias = "uint16_t", size = 2 },
    uint32 = { alias = "uint32_t", size = 4 },
    four_c_c = { alias = "FourCC", size = 4 }
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

local tagClasses = {
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
    "vehicle",
    "weapon",
    "wind",
    "weapon_hud_interface"
}

local function camelCaseToSnakeCase(str)
    if str then
        -- convert camelCase to snake_case but keep numbers
        str = str:sub(1, 1):lower() .. str:sub(2)
        local new = str:gsub("([A-Z])", function(c)
            return "_" .. c:lower()
        end)
        if new:sub(1, 1) == "_" then
            new = new:sub(2)
        end
        return new
    end
end

local function dashAndSentenceToSnakeCase(str)
    -- convert dash-and-sentence to snake_case
    return str:gsub("([A-Z])", function(c)
        return "_" .. c:lower()
    end):gsub("-", "_"):gsub(" ", "_"):gsub("'", "")
end

local function normalToSnakeCase(str)
    if str then
        -- convert normal sentence to snake_case
        return str:gsub(" ", "_"):gsub("[(|)|']", "")
    end
end

local function snakeCaseToCamelCase(str)
    if str then
        -- convert snake_case to camelCase
        local new = str:gsub("_([a-z])", function(c)
            return c:upper()
        end)
        -- capitalize first letter
        new = new:sub(1, 1):upper() .. new:sub(2)
        return new
    end
    return nil
end

local function snakeCaseToLowerCamelCase(str) 
    str = snakeCaseToCamelCase(str)
    if str then
        return str:sub(1, 1):lower() .. str:sub(2)
    end
    return nil
end

---Parse a struct definition
---@param structDefinition DefinitionElement
---@return Struct
local function parseStruct(structDefinition)
    local struct = {
        name = snakeCaseToCamelCase(structDefinition.class) or structDefinition.name,
        size = structDefinition.size,
        inherits = structDefinition.inherits,
        fields = {}
    }

    for fieldIndex, field in pairs(structDefinition.fields) do
        local structField = {}

        local fieldType = camelCaseToSnakeCase(field.type)
        if commonTypes[fieldType] then
            structField.type = commonTypes[fieldType].alias
        else
            structField.type = field.type
        end

        if field.name then
            local fieldName = normalToSnakeCase(camelCaseToSnakeCase(field.name))
            if fieldName and fieldName:find("%d") == 1 then
                fieldName = "_" .. fieldName
            end
            structField.name = fieldName
        end
        
        structField.struct = field.struct
        structField.size = field.size or field.count
        structField.cacheOnly = field.cache_only or false

        if field.bounds then
            structField.size = 2
        end

        struct.fields[fieldIndex] = structField
    end

    return struct
end

---Parse a enum definition
---@param enumDefinition DefinitionElement
---@return Enum
local function parseEnum(enumDefinition)
    local enum = {name = enumDefinition.name, values = {}}
    if enumDefinition.options then
        for index, value in pairs(enumDefinition.options) do
            if type(value) == "table" then
                local val = value
                if val.name then
                    val.name = camelCaseToSnakeCase(enumDefinition.name) .. "_" .. dashAndSentenceToSnakeCase(val.name)
                end
                enum.values[index] = val
            else 
                enum.values[index] = { name = value }
            end
        end
    end
    return enum
end

---Parse a bitfield definition
---@param bitfieldDefinition DefinitionElement
---@return BitfieldStruct
local function parseBitfield(bitfieldDefinition)
    local bitfield = {
        name = bitfieldDefinition.name, 
        width = bitfieldDefinition.width, 
        fields = {}
    }
    if bitfieldDefinition.fields then
        for _, field in pairs(bitfieldDefinition.fields) do
            if type(field) == "table" then
                local val = field
                if val.name then
                    local fieldName = dashAndSentenceToSnakeCase(val.name)
                    if fieldName and fieldName:find("%d") == 1 then
                        fieldName = "_" .. fieldName
                    end
                    val.name = fieldName
                end
                table.insert(bitfield.fields, val)
            else
                local fieldName = dashAndSentenceToSnakeCase(field)
                if fieldName and fieldName:find("%d") == 1 then
                    fieldName = "_" .. fieldName
                end
                table.insert(bitfield.fields, { name = fieldName })
            end
        end
    end
    return bitfield
end

---Convert an Invader element to type definition
---@param definition DefinitionElement
---@return TagDefinition
local function parseDefinition(definitionName, definition)
    local enums = {}
    local bitfields = {}
    local structs = {}
    
    for _, element in pairs(definition) do
        if element.type == "enum" then
            enums[#enums + 1] = parseEnum(element)
        elseif element.type == "bitfield" then
            bitfields[#bitfields + 1] = parseBitfield(element)
        elseif element.type == "struct" then
            structs[#structs + 1] = parseStruct(element)
        end
    end

    return {enums = enums, bitfields = bitfields, structs = structs}
end

local function getDependencies(definitions)
    local dependencies = {}

    local findType = function(typeName)
        if commonTypes["" .. camelCaseToSnakeCase(typeName)] then
            return "common"
        elseif commonEnums["" .. camelCaseToSnakeCase(typeName)] then
            return "common"
        elseif commonBitfields["" .. camelCaseToSnakeCase(typeName)] then
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
                elseif field.type ~= "pad" then
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
    camelCaseToSnakeCase = camelCaseToSnakeCase,
    normalToSnakeCase = normalToSnakeCase,
    snakeCaseToCamelCase = snakeCaseToCamelCase,
    snakeCaseToLowerCamelCase = snakeCaseToLowerCamelCase,
    dashAndSentenceToSnakeCase = dashAndSentenceToSnakeCase,
    parseDefinition = parseDefinition,
    getDependencies = getDependencies,
    types = commonTypes,
    commonEnums = commonEnums,
    commonBitfields = commonBitfields,
    tagClasses = tagClasses,
}
