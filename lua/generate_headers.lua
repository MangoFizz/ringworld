-- SPDX-License-Identifier: GPL-3.0-only

local argparse = require "argparse"
local glue = require "glue"
local json = require "json"
local parser = require "tag_definition_parser"

local argsParser = argparse("Tag definitions structures generator", "Convert Invader tag definitions to C structs")
argsParser:argument("output_directory", "Headers output directory"):args(1)
argsParser:argument("files", "Tag definitions files"):args("*")

local args = argsParser:parse()
local files = args.files
local outputDirectory = args.output_directory

---Generate a header file for a tag definition
---@param definitionName string
---@param tagDefinition TagDefinition
---@param dependencies table<string, table<string, boolean>>
local function generateHeader(definitionName, tagDefinition, dependencies)
    local structContent = ""

    local function add(text)
        structContent = structContent .. text
    end

    local function ident(n)
        add(string.rep(" ", n*4))
    end

    local function addType(typeName)
        add("T<" .. typeName .. ">")
    end

    local memes = {
        ["U_I_"] = "UI_"
    }

    local function fixMemes(text)
        for k, v in pairs(memes) do
            text = text:gsub(k, v)
        end
        return text
    end

    add([[
// SPDX-License-Identifier: GPL-3.0-only
// This file is auto-generated. DO NOT EDIT!

#ifndef RINGWORLD__TAG__DEFINITIONS__]] .. definitionName:upper() .. [[_H
#define RINGWORLD__TAG__DEFINITIONS__]] .. definitionName:upper() .. [[_H

#include "../../types/types.h"
#include "../tag.h"
#include "enum.h"
#include "bitfield.h"

#pragma pack(push)
#pragma pack(1)

]])

    if(dependencies) then
        for dependency, _ in pairs(dependencies) do
            if dependency ~= "common" and dependency ~= definitionName then
                add("#include \"" .. dependency .. ".h\"\n")
            end
        end
    end

    for _, enum in ipairs(tagDefinition.enums) do
        local enumPrefix = fixMemes(parser.camelCaseToSnakeCase(enum.name):upper() .. "_")
        add("typedef enum PACKED_ENUM " .. enum.name .. " {\n")
        for index, value in ipairs(enum.values) do
            local valueName = enumPrefix .. parser.normalToSnakeCase(value.name:upper())
            ident(1)
            if index == 1 then
                add(valueName .. " = 0,\n")
            else
                add(valueName .. ",\n")
            end
        end
        local enum_count = enumPrefix .. "MAX"
        ident(1)
        add(enum_count .. ", \n")
        local enum_size = enumPrefix .. "SIZE"
        ident(1)
        add(enum_size .. " = 0xFFFF \n")
        add("} " .. enum.name .. ";\n\n")
    end

    for _, bitfield in ipairs(tagDefinition.bitfields) do
        local bitfieldType
        local width = bitfield.width
        if width == 8 then
            bitfieldType = "uint8_t"
        elseif width == 16 then
            bitfieldType = "uint16_t"
        else 
            bitfieldType = "uint32_t"
        end

        add("typedef struct " .. parser.snakeCaseToCamelCase(bitfield.name) .. " {\n")
        for _, field in ipairs(bitfield.fields) do
            ident(1)
            add(bitfieldType .. " " .. field.name .. " : 1;\n")
        end
        add("} " .. parser.snakeCaseToCamelCase(bitfield.name) .. ";\n")
        add("_Static_assert(sizeof(" .. bitfield.name .. ") == sizeof(" .. bitfieldType .. "));\n\n")
    end
    
    for _, struct in pairs(tagDefinition.structs) do
        ident(0)
        add("typedef struct " .. parser.snakeCaseToCamelCase(struct.name) .. " { \n")
        if struct.inherits then
            ident(1)
            add(parser.snakeCaseToCamelCase(struct.inherits) .. " base; \n")
        end
        for _, field in ipairs(struct.fields) do
            ident(1)
            if not field.type then
                add("byte " .. field.name .. "[" .. field.size .. "];\n")
            elseif field.type == "pad" then
                add("char pad_" .. string.len(structContent) .. "[" .. field.size .. "];\n")
            else
                if field.type == "TagBlock" then
                    add("struct { uint32_t count; struct " .. parser.snakeCaseToCamelCase(field.struct) .. " *elements; TagBlockDefinition *definition; }")
                else
                    add(field.type)
                end
                
                -- Field name
                add(" ")
                if field.pointer then
                    add("*")
                end
                add(field.name)
                if field.size then
                    add("[" .. field.size .. "]")
                end
                
                add(";\n")
            end
        end
        ident(0)
        add("} " .. parser.snakeCaseToCamelCase(struct.name) .. ";\n")
        ident(0)
        add("_Static_assert(sizeof(" .. parser.snakeCaseToCamelCase(struct.name) .. ") == " .. struct.size .. ");\n\n")
    end        
    add([[

#pragma pack(pop)

#endif

]])
    return structContent
end

local definitions = {}

for _, file in ipairs(files) do
    local fileName = file:match("([^/]+)$")
    local definitionName = fileName:match("^(.+)%..+$")
    local definition = json.decode(glue.readfile(file))
    local parsedDefinition = parser.parseDefinition(definitionName, definition)
    definitions[definitionName] = parsedDefinition
end

local dependencies = parser.getDependencies(definitions)

for definitionName, definition in pairs(definitions) do
    -- generate and write header
    local definitionDependencies = dependencies[definitionName]

    -- Generate little endian header
    local header = generateHeader(definitionName, definition, definitionDependencies)
    glue.writefile(outputDirectory .. "/ringworld/tag/definitions/" .. definitionName .. ".h", header, "t")
end
