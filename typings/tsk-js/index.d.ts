declare module 'tsk-js' {
    export type ImgInfo = TskJs.ImgInfo;
    export type TskOptions = TskJs.TskOptions;
    export type PartitionInfo = TskJs.PartitionInfo;
    export type ImgFile = TskJs.ImgFile;
    export type DiskAction = TskJs.DiskAction;
    export type TimelineCallback = TskJs.TimelineCallback;
    export type SearchCallback = TskJs.SearchCallback;
    export type TimelineItem = TskJs.TimelineItem;
    export default TskJs.TSK;
}

declare namespace TskJs {
    class TSK {
        /**
         * Constructor
         * @param imgfile Path to an image file
         */
        constructor(imgfile: string);
 
        analyze(): ImgInfo;
        list(opts?: TskOptions): Array<ImgFile>;
        get(opts?: TskOptions): Buffer;
        timeline(cb?: TimelineCallback, opts?: TskOptions): Array<TimelineItem>;
        search(needle: string, cb?: SearchCallback, opts?: TskOptions): void;
    }

    export interface TskConstructor {
        new (imgfile: string): TSK;
        prototype: TSK;
    }

    export interface ImgInfo {
        type: "disk" | "partition";
        partitions?: Array<PartitionInfo>;
    }

    export interface TskOptions {
        imgaddr?: number;
        inode?: number;
    }

    export interface PartitionInfo {
        description: string;
        start: number;
        end: number;
        size:number;
        hasFs: boolean;
    }

    export interface ImgFile {
        path: string;
        name: string;
        allocated: boolean;
        type: "directory" | "virtual" | "register" | "unknown";
        metaType: "directory" | "virtual" | "register" | "unknown";
        inode: number;
        metaAddr: string;
        hasChildren?: boolean;
    }

    export interface SearchResult {
        file: string;
        context: string;
        character: string;
    }

    export type DiskAction = "access" | "modify" | "creation" | "change";
    export type TimelineCallback = (list: Array<TimelineItem>) => void;
    export type SearchCallback =
        (file: SearchResult, index: number) => void;

    export interface TimelineItem extends ImgFile {
        actions: Array<DiskAction>;
        fileNameFlag: Boolean;
        date: Date;
    }
}